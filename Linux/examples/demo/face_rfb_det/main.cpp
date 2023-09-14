/****************************************************************************
*   This is a demo,test nn api
*
*   Neural Network application project entry file
*
*   2019.8 author zxw
****************************************************************************/
/*-------------------------------------------
                Includes
-------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "nn_sdk.h"
#include "nn_util.h"
#include <pthread.h>
#include "jpeglib.h"
#include <iostream>
#include "face_rfb_det.h"

/******************************************************************************

******************************************************************************/
static void *context = NULL;
char * jpath = NULL;
static unsigned char *rawdata = NULL;
//////////////////for input/output dma////////////////////
static unsigned char *outbuf = NULL;
static unsigned char *inbuf = NULL;
static int use_dma = 0;
static amlnn_input_mode_t inmode;
//static unsigned char *inbuf2 = NULL;
static int input_width = 0, input_high = 0;
static int display_width = 640, display_high = 480;
///////////////////////////////////////////////////////////
nn_input inData;
//for usbcamera
extern pthread_mutex_t mutex_data;
extern unsigned char *rgbbuf;
extern char *fbp;

int run_network(void *qcontext, unsigned char *qrawdata,int fbmode,unsigned char *fbbuf)
{

	face_rfb_detect_out_t *face_rfb_det_out = NULL;
	nn_output *outdata = NULL;

	int sz=1;
	int j;
	unsigned int i=0;
	int ret = 0;
	float *buffer = NULL;
	int cols=416;
	int rows=416;
	FILE *fp,*File;
	aml_output_config_t outconfig;

	//tmsStart = get_perf_count();
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
		inData.input=qrawdata;
		ret = aml_module_input_set(qcontext,&inData);
		if (ret != 0)
		{
			printf("aml_module_input_set error\n");
		}
	}

	outconfig.format = AML_OUTDATA_FLOAT32;
	outdata = (nn_output*)aml_module_output_get(qcontext,outconfig);
	face_rfb_det_out = (face_rfb_detect_out_t*)postprocess_rfb_facedet(outdata);
	
	if (face_rfb_det_out == NULL)
	{
		printf("aml_module_output_get error\n");
		return -1;
	}
	printf("detect number =%d \n",face_rfb_det_out->detNum );
	for (i = 0; i < face_rfb_det_out->detNum; i++)
	{
		printf("face_number:%d : box.x = %f , box.y = %f, box.w = %f, box.h = %f \n", i, face_rfb_det_out->facebox[i].x * 320, face_rfb_det_out->facebox[i].y * 320, face_rfb_det_out->facebox[i].w * 320, face_rfb_det_out->facebox[i].h * 320);
	}
		
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
		
	printf("the input type should be 320*320*3\n");
	input_width = 320;
	input_high = 320;
	config.modelType = CAFFE;
	qcontext = aml_module_create(&config);
	if (qcontext == NULL)
	{
		printf("amlnn_init is fail\n");
		return NULL;
	}
	inData.input_index = 0;   //this value is index of input,begin from 0
	inData.size = 320*320*3;
	inData.input_type = RGB24_RAW_DATA;
	inData.info.mean[0] = 104;    //the input mean will set as this;
	inData.info.mean[1] = 117;
	inData.info.mean[2] = 123;
			
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
		thread_args = (void*)argv[3];
	}

	else if (strcmp(argv[2], "video") == 0)
	{
		inmode = AML_IN_VIDEO;
		
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
