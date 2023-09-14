/****************************************************************************
*
*    Copyright (c) 2019  by amlogic Corp.  All rights reserved.
*
*    The material in this file is confidential and contains trade secrets
*    of amlogic Corporation. No part of this work may be disclosed,
*    reproduced, copied, transmitted, or used in any way for any purpose,
*    without the express written permission of amlogic Corporation.
*
***************************************************************************/
/*-------------------------------------------
                Includes
-------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "nn_sdk.h"
#include "nn_demo.h"
#include "nn_util.h"
#include <pthread.h>
#include "jpeglib.h"
#include <iostream>

#define NBG_FROM_MEMORY
static const char *sdkversion = "v1.8.2,2021.06";
static void *context = NULL;
char * jpath = NULL;
static unsigned char *rawdata = NULL;
//for input/output dma
static unsigned char *outbuf = NULL;
static unsigned char *inbuf = NULL;
static int use_dma = 0;
static amlnn_input_mode_t inmode;
static int input_width = 0, input_high = 0;
static int display_width = 640, display_high = 480;

nn_input inData;
//for usbcamera
extern pthread_mutex_t mutex_data;
extern unsigned char *rgbbuf;
extern char *fbp;

static int _jpeg_to_bmp
    (
    FILE * inputFile,
    unsigned char* bmpData,
    unsigned int bmpWidth,
    unsigned int bmpHeight,
    unsigned int channel
    )
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    unsigned char *point = NULL;
    unsigned long width, height;
    unsigned short depth = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo,inputFile);
    jpeg_read_header(&cinfo,TRUE);

    cinfo.dct_method = JDCT_IFAST;

    if (bmpData == NULL)
    {
        return -1;
    }
    else
    {
        jpeg_start_decompress(&cinfo);

        width  = cinfo.output_width;
        height = cinfo.output_height;
        depth  = cinfo.output_components;
        if (width * height * depth != bmpWidth * bmpHeight * channel)
        {
           printf("wrong jpg file , the jpg file size should be %u %u %u\n",
               bmpWidth, bmpHeight, channel);
           return -1;
        }

        buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr)&cinfo, JPOOL_IMAGE, width*depth, 1);

        point = bmpData;

        while (cinfo.output_scanline < height)
        {
            jpeg_read_scanlines(&cinfo, buffer, 1);
            memcpy(point, *buffer, width * depth);
            point += width * depth;
        }

        jpeg_finish_decompress(&cinfo);
    }

    jpeg_destroy_decompress(&cinfo);

    return 0;
}

unsigned char *get_jpeg_rawData(const char *name,unsigned int width,unsigned int height)
{
    FILE *bmpFile;
    unsigned char *bmpData;
    unsigned int sz,w,h,c;
    int status;

    bmpFile = NULL;
    bmpData = NULL;
    w = width;
    h = height;
    c = 3;
    sz = w*h*3;

    bmpFile = fopen( name, "rb" );
	if (bmpFile == NULL)
	{
		printf("null returned\n");
		goto final;
	}

    bmpData = (unsigned char *)malloc(sz * sizeof(char));
	if (bmpData == NULL)
	{
		printf("null returned\n");
		goto final;
	}
    memset(bmpData, 0, sz * sizeof(char));

    status = _jpeg_to_bmp( bmpFile, bmpData, w, h, c);
    if (status == -1)
    {
        free(bmpData);
        fclose(bmpFile);
        return NULL;
    }

final:
    if (bmpFile)fclose(bmpFile);
    return bmpData;
}

void print_help(void)
{
	printf("SDK version:%s\n",sdkversion);
	printf("Useage:\n");
	printf("input picture:nnsdk nbgfile-path networkType jpeg-path\n");
	printf("input camera: nnsdk nbgfile-path networkType camera\n");
	printf("input video:  nnsdk nbgfile-path networkType video video-path(avi,mp4,flv and so on)\n");
	printf("The network type defined as below:\n");
	printf("IMAGE_CLASSIFY : 0\n");
	printf("OBJECT_DETECT : 1\n");
	printf("FACE_DETECTION : 2\n");
	printf("FACE_LANDMARK_5 : 3\n");
	printf("FACE_LANDMARK_68 : 4\n");
	printf("FACE_RECOGNIZE : 5\n");
	printf("FACE_COMPARISION : 6\n");
	printf("FACE_AGE : 7\n");
	printf("FACE_GENDER : 8\n");
	printf("FACE_EMOTION : 9\n");
	printf("BODY_POSE : 10\n");
	printf("FINGER_POSE : 11\n");
	printf("HEAD_DETECTION : 12\n");
	printf("CARPLATE_DETECTION : 13\n");
	printf("CARPLATE_RECOG : 14\n");
	printf("TEXT_DETECTION : 15\n");
	printf("IMAGE_SR : 16\n");
	printf("IMAGE_SEGMENTATION : 17\n");
	printf("PERSON_DETECT : 18\n");
	printf("CUSTOM_NETWORK: 99\n");
	printf("In this version,just support netType:0,1,2,3,4,5,6,7,8,9,10,12,15,17,18,the CUSTOM_NETWORK used for customer defined network\n");
}

float Float16ToFloat32(const signed short* src , float* dst ,int lenth)
{
	signed int t1;
	signed int t2;
	signed int t3;
	float out;
	int i;
	for (i = 0 ;i < lenth ;i++)
	{
		t1 = src[i] & 0x7fff;                       // Non-sign bits
		t2 = src[i] & 0x8000;                       // Sign bit
		t3 = src[i] & 0x7c00;                       // Exponent

		t1 <<= 13;                              // Align mantissa on MSB
		t2 <<= 16;                              // Shift sign bit into position

		t1 += 0x38000000;                       // Adjust bias

		t1 = (t3 == 0 ? 0 : t1);                // Denormals-as-zero

		t1 |= t2;
		*((unsigned int*)&out) = t1;                 // Re-insert sign bit
		dst[i] = out;

	}
	return out;
}

float *dtype_To_F32(nn_output * outdata ,int sz)
{
	int stride, fl, i, zeropoint;
	float scale;
	unsigned char *buffer_u8 = NULL;
	signed char *buffer_int8 = NULL;
	signed short *buffer_int16 = NULL;
	float *buffer_f32 = NULL;

	buffer_f32 = (float *)malloc(sizeof(float) * sz );

	if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_UINT8)
	{
		stride = (outdata->out[0].size)/sz;
		scale = outdata->out[0].param->quant_data.affine.scale;
		zeropoint =  outdata->out[0].param->quant_data.affine.zeroPoint;

		buffer_u8 = (unsigned char*)outdata->out[0].buf;
		for (i = 0; i < sz; i++)
		{
			buffer_f32[i] = (float)(buffer_u8[stride * i] - zeropoint) * scale;
		}
	}

	else if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_INT8)
	{
		buffer_int8 = (signed char*)outdata->out[0].buf;
		if (outdata->out[0].param->quant_data.dfp.fixed_point_pos >= 0)
		{
			fl = 1 << (outdata->out[0].param->quant_data.dfp.fixed_point_pos);
			for (i = 0; i < sz; i++)
			{
				buffer_f32[i] = (float)buffer_int8[i] * (1.0/(float)fl);
			}
		}
		else
		{
			fl = 1 << (-outdata->out[0].param->quant_data.dfp.fixed_point_pos);
			for (i = 0; i < sz; i++)
				buffer_f32[i] = (float)buffer_int8[i] * ((float)fl);
		}
	}

	else if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_INT16)
	{
		buffer_int16 =	(signed short*)outdata->out[0].buf;
		if (outdata->out[0].param->quant_data.dfp.fixed_point_pos >= 0)
		{
			fl = 1 << (outdata->out[0].param->quant_data.dfp.fixed_point_pos);
			for (i = 0; i < sz; i++)
			{
				buffer_f32[i] = (float)((buffer_int16[i]) * (1.0/(float)fl));
			}
		}
		else
		{
			fl = 1 << (-outdata->out[0].param->quant_data.dfp.fixed_point_pos);
			for (i = 0; i < sz; i++)
				buffer_f32[i] = (float)((buffer_int16[i]) * ((float)fl));
		}
	}
	else if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_FP16 )
	{
		buffer_int16 = (signed short*)outdata->out[0].buf;

		Float16ToFloat32(buffer_int16 ,buffer_f32 ,sz);
	}

	else if (outdata->out[0].param->data_format == NN_BUFFER_FORMAT_FP32)
	{
		memcpy(buffer_f32, outdata->out[0].buf, sz);
	}
	else
	{
		printf("Error: currently not support type, type = %d\n", outdata->out[0].param->data_format);
	}
	return buffer_f32;
}

int run_network(void *qcontext, unsigned char *qrawdata,int fbmode,unsigned char *fbbuf)
{
	img_classify_out_t *cls_out = NULL;
	obj_detect_out_t *obj_detect_out=NULL;
	nn_output *outdata = NULL;
    aml_module_t modelType;
	int sz=1;
	int j;
	unsigned int i=0;
	int ret = 0;
	float *buffer = NULL;
	int cols=416;
	int rows=416;
	FILE *fp,*File;
	aml_output_config_t outconfig;

	if (use_dma == 1)
	{
		if (qrawdata != inbuf )
		{
			memcpy(inbuf,qrawdata,input_high * input_width * 3);
			aml_util_flushTensorHandle(qcontext,AML_INPUT_TENSOR);  //!!note,flush the input buffer in dma mode
		}
	}
	else
	{
		inData.input = qrawdata;
			
		ret = aml_module_input_set(qcontext,&inData);
		if (ret != 0)
		{
			printf("aml_module_input_set error\n");
		}
	}

	#ifndef USE_OPENCV
	if(rawdata != NULL)
	{
		free(rawdata);
		rawdata = NULL;
	}
	#endif

	outconfig.typeSize = sizeof(aml_output_config_t);
	modelType = IMAGE_CLASSIFY;
	outconfig.format = AML_OUTDATA_FLOAT32;
	outdata = (nn_output*)aml_module_output_get(qcontext,outconfig);
	if (outdata == NULL)
	{
		printf("aml_module_output_get error\n");
		return -1;
	}
	cls_out = (img_classify_out_t*)post_process_all_module(modelType,outdata);
	if (cls_out == NULL)
	{
		printf("post_process_module error\n");
		return -1;
	}
	if(fbmode == 0)
	{
		for (i = 0; i < 5; i++)
		{
			printf("top %d:score--%f,class--%d\n",i,cls_out->score[i],cls_out->topClass[i]);
		}
	}
	else
	{
	}

	#ifdef USE_OPENCV
	if(rawdata != NULL)
	{
		free(rawdata);
		rawdata = NULL;
	}
	#endif
	return ret;
}

void* init_network(int argc,char **argv)
{
	const char *jpeg_path = NULL;
    int size=0;
	aml_config config;
    tensor_info* inptr;
    tensor_info* outptr;
    void *qcontext = NULL;

	memset(&config,0,sizeof(aml_config));
	FILE *fp,*File;

	config.path = (const char *)argv[1];
	config.nbgType = NN_NBG_FILE;
	printf("%d\n",argv[2][1]);
	
	printf("the input type should be 224*224*3\n");
	input_width = 224;
	input_high = 224;

	config.modelType = TENSORFLOW;
	qcontext = aml_module_create(&config);
	if (qcontext == NULL)
	{
		printf("amlnn_init is fail\n");
		return NULL;
	}
	inData.input_index = 0;   //this value is index of input,begin from 0
	inData.size = 224*224*3;
	inData.input_type = RGB24_RAW_DATA;

	
	if(config.nbgType == NN_NBG_MEMORY && config.pdata != NULL)
	{
		free((void*)config.pdata);
	}
	return qcontext;
}

int destroy_network(void *qcontext)
{
	if(outbuf)aml_util_freeAlignedBuffer(outbuf);
	if(inbuf)aml_util_freeAlignedBuffer(inbuf);

	int ret = aml_module_destroy(qcontext);
	return ret;
}
#ifdef USE_OPENCV
int resize_input_data(unsigned char *indata,unsigned char *outdata)
{
	cv::Mat inImage = cv::Mat(display_high, display_width, CV_8UC3);
	cv::Mat dstImage = cv::Mat(input_high, input_width, CV_8UC3);
	inImage.data = indata;
	dstImage.data = outdata;
	cv::resize(inImage,dstImage,dstImage.size());
	return 0;
}
#endif
void* net_thread_func(void *args)
{
	jpath = (char*)args;
	int ret = 0;
	#ifdef USE_OPENCV
	unsigned char *dup_rgbbuf = NULL;
	unsigned char *input_data = NULL;
	char cmd[128];
	char img_name[64];
	char *ptr;
	#endif


	if(inmode == AML_IN_PICTURE)
	{
		rawdata = get_jpeg_rawData(jpath,input_width,input_high); //this size should set by network 
		ret = run_network(context,rawdata,AML_IN_PICTURE,NULL);
	}	
	else if(inmode == AML_IN_VIDEO)
	{
		printf("now in video mode,make sure the ffmpeg and opencv is ok,we will parse it---\n");
		#ifdef USE_OPENCV
		Mat img;
		Mat dst = Mat(416,416,CV_8UC3);
		std::string iname;
		int index=1;
	
		memset(cmd,0,sizeof(cmd));
		memset(img_name,0,sizeof(img_name));
		system("mkdir tmp");
		sprintf(cmd,"ffmpeg -i %s -qscale:v 2 -r 24 ",jpath);
		ptr=strcat(cmd,"tmp/image%5d.bmp");
		system(ptr);
		sprintf(img_name,"tmp/image%05d.bmp",index);

		while (1)
		{
			sprintf(img_name,"tmp/image%05d.bmp",index);
			if((access(img_name,F_OK)) < 0)
			{
				break;
			}
			img=imread(img_name,199);
			resize(img,dst,dst.size());
			ret = run_network(context,dst.data,AML_IN_VIDEO,(unsigned char*)img_name);
			index++;	 
		}
		system("ffmpeg -f image2 -i tmp/image%5d.bmp -b:v 5626k videoout.mp4");
		system("rm tmp -r");
		#endif
	}
		
	else
	{
		#ifdef USE_OPENCV
		dup_rgbbuf = (unsigned char*)malloc(display_width * display_high * 3);
		input_data = (unsigned char*)malloc(input_width * input_high * 3);
		
		while(1)
		{
			if(rgbbuf != NULL)
			{
				pthread_mutex_lock(&mutex_data);
				memcpy(dup_rgbbuf,rgbbuf,display_width*display_high*3);
				pthread_mutex_unlock(&mutex_data);
			}
			resize_input_data(dup_rgbbuf,input_data);
			ret = run_network(context,input_data,AML_IN_CAMERA,dup_rgbbuf);
		}
		#endif
	}
	
    ret = destroy_network(context);
	if (ret != 0)
	{
		printf("aml_module_destroy error\n");
	}
	return (void*)0;
}

/*-------------------------------------------
                  Main Functions
-------------------------------------------*/
int main(int argc,char **argv)
{
	int ret = 0,i=0;
	pthread_t tid[2];
	int customID,powerStatus;
	void *thread_args = NULL;

	if (argc < 3)
    {
        print_help();
        return -1;
    }
	if (strcmp(argv[1], "--help") == 0)
	{
		print_help();
		return 0;
	}
	
	context = init_network(argc,argv);
	
	if (strcmp(argv[2], "camera") == 0)
	{
		printf("please make sure the usb camera is ready,and the hdmi is connet to display device\n");
		inmode = AML_IN_CAMERA;
		pthread_mutex_init(&mutex_data,NULL);
		init_fb();
		if (0 != pthread_create(&tid[0],NULL,camera_thread_func,NULL)) 
		{
			fprintf(stderr, "Couldn't create thread func\n");
			return -1;
		}
		thread_args = (void*)argv[2];
	}

	else if (strcmp(argv[2], "video") == 0)
	{
		inmode = AML_IN_VIDEO;
		if (argc < 4)
		{
			print_help();
			return -1;
		}
		thread_args = (void*)argv[3];
	} 
	
	else
	{
		inmode = AML_IN_PICTURE;
		thread_args = (void*)argv[2];
	}


	if (0 != pthread_create(&tid[1],NULL,net_thread_func,thread_args))
	{
		fprintf(stderr, "Couldn't create thread func\n");
		return -1;
	}

	if(inmode != AML_IN_CAMERA)
	{
		pthread_join(tid[1], NULL);
	}
	else
	{
		while(1)
		{
			for (i=0;i<2;i++)
			{
				pthread_join(tid[i], NULL);
			}
		}
	}

    return ret;
}
