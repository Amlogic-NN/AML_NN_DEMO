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
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "nn_sdk.h"
#include "postprocess.h"
#include "utils.h"
#include "jpeglib.h"

const char *coco_names[] = {"person","bicycle","car","motorbike","aeroplane","bus","train",
				"truck","boat","traffic light","fire hydrant","stop sign","parking meter",
				"bench","bird","cat","dog","horse","sheep","cow","elephant","bear","zebra",
				"giraffe","backpack","umbrella","handbag","tie","suitcase","frisbee","skis",
				"snowboard","sports ball","kite","baseball bat","baseball glove","skateboard",
				"surfboard","tennis racket","bottle","wine glass","cup","fork","knife","spoon",
				"bowl","banana","apple","sandwich","orange","broccoli","carrot","hot dog","pizza",
				"donut","cake","chair","sofa","pottedplant","bed","diningtable","toilet","tvmonitor",
				"laptop","mouse","remote","keyboard","cell phone","microwave","oven","toaster","sink",
				"refrigerator","book","clock","vase","scissors","teddy bear","hair drier","toothbrush"};

///////////////////////////////////////////////////////////
#define BILLION 1000000000
static uint64_t tmsStart, tmsEnd, msVal_inference, usVal_inference;
static uint64_t get_perf_count()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)((uint64_t)ts.tv_nsec + (uint64_t)ts.tv_sec * BILLION);
}

int main(int argc,char **argv)
{
	if (argc < 5)
	{
        printf("Usage:\n");
        printf("argv[0]: exe_file\n");
        printf("argv[1]: xxx.nb/adla\n");
        printf("argv[2]: xxx.jpg/jpeg\n");
        printf("argv[3]: input_mode(0:rgb, 1:bin, 2:dma)\n");
        printf("argv[4]: output_mode(0:fp32, 1:raw, 2:dma)\n");
		return -1;
	}

    int ret = 0;
    int input_mode = 0;
    int output_mode = 0;
    const char *model_path = NULL;

    void *context = NULL;
    aml_config config;

    nn_input inData;
    const char *input_path = NULL;
    unsigned char *rawdata = NULL;
    int input_width = 0;
    int input_height = 0;
    int input_size = 0;
    int offset = 0;
    unsigned char *tmpdata = NULL;
    aml_memory_config_t mem_config;
    unsigned char *inbuf = NULL;
    unsigned char *outbuf[ADDRESS_MAX_NUM] = {NULL};
    int insize = 0;
    int outsize[ADDRESS_MAX_NUM] = {0};

    nn_output *outdata = NULL;
    aml_output_config_t outconfig;

    tensor_info *input_tensor;
    tensor_info *output_tensor;

    yolov5_out_t *yolov5_out = NULL;

    model_path = (const char *)argv[1];
    input_path = (const char *)argv[2];
    input_mode = std::stoi(argv[3]);
    output_mode = std::stoi(argv[4]);

    input_width = 640;
    input_height = 384;
    input_size = input_width * input_height * 3;


    /* get tensor info */
    FILE *fp = fopen(model_path,"rb");
    fseek(fp,0,SEEK_END);
    int model_size = (int)ftell(fp);
    rewind(fp);
    config.pdata = (char *)calloc(1,model_size);
    fread((void*)config.pdata,1,model_size,fp);
    config.nbgType = NN_NBG_MEMORY;
    config.length = model_size;
    fclose(fp);

    input_tensor = aml_util_getInputTensorInfo(config.pdata);
    output_tensor = aml_util_getOutputTensorInfo(config.pdata);
    if (input_tensor == NULL || output_tensor == NULL)
    {
        std::cout << "aml_util_getTensorInfo fail." << std::endl;
        return -1;
    }
    if (config.nbgType == NN_NBG_MEMORY && config.pdata != NULL)
    {
        free((void*)config.pdata);
    }

    std::cout << "input tensors: " << input_tensor->num << std::endl;
    std::cout << "output tensors: " << output_tensor->num << std::endl;

    std::cout << "input_" << 0 << " info: "
              << "n: " << input_tensor->info[0].sizes_of_dim[3] << " "
              << "c: " << input_tensor->info[0].sizes_of_dim[2] << " "
              << "h: " << input_tensor->info[0].sizes_of_dim[1] << " "
              << "w: " << input_tensor->info[0].sizes_of_dim[0] << " "
              << "zp: " << input_tensor->info[0].TF_zeropoint << " "
              << "scale: " << input_tensor->info[0].TF_scale << " "
              << "fl: " << input_tensor->info[0].fixed_point_pos << " "
              << "format: " << input_tensor->info[0].data_format << " "
              << std::endl;

    for (int i = 0; i < output_tensor->num; i++)
    {
        std::cout << "output_" << i << " info: "
                  << "n: " << output_tensor->info[i].sizes_of_dim[3] << " "
                  << "c: " << output_tensor->info[i].sizes_of_dim[2] << " "
                  << "h: " << output_tensor->info[i].sizes_of_dim[1] << " "
                  << "w: " << output_tensor->info[i].sizes_of_dim[0] << " "
                  << "zp: " << output_tensor->info[i].TF_zeropoint << " "
                  << "scale: " << output_tensor->info[i].TF_scale << " "
                  << "fl: " << output_tensor->info[i].fixed_point_pos << " "
                  << "format: " << output_tensor->info[i].data_format << " "
                  << std::endl;
    }


    /* model init */
    memset(&config, 0, sizeof(aml_config));

    if (input_mode == 2)
    {
        insize = input_tensor->info[0].sizes_of_dim[3] * input_tensor->info[0].sizes_of_dim[2] * input_tensor->info[0].sizes_of_dim[1] * input_tensor->info[0].sizes_of_dim[0];
        inbuf = aml_util_mallocAlignedBuffer(NULL,insize,&mem_config);
        config.inOut.inAddr[0] = inbuf;
    }
    if (output_mode == 2)
    {
        for (int i = 0; i < output_tensor->num; i++)
        {
            outsize[i] = output_tensor->info[i].sizes_of_dim[3] * output_tensor->info[i].sizes_of_dim[2] * output_tensor->info[i].sizes_of_dim[1] * output_tensor->info[i].sizes_of_dim[0];
            outbuf[i] = aml_util_mallocAlignedBuffer(NULL,outsize[i],&mem_config);
            config.inOut.outAddr[i] = outbuf[i];
        }
    }

    config.nbgType = NN_NBG_FILE;
    config.path = model_path;
    config.modelType = ONNX;
    config.typeSize = sizeof(aml_config);
    context = aml_module_create(&config);
    if (NULL == context)
    {
        std::cout << "aml_module_create fail." << std::endl;
        return -1;
    }

    /* setinput */
    rawdata = get_jpeg_rawData(input_path, input_width, input_height);
    inData.input_index = 0;
    inData.size = input_size;
    inData.info.int16_type = 0;
    if (input_mode == 0)
    {
        inData.input_type = RGB24_RAW_DATA;
        inData.input = rawdata;
        ret = aml_module_input_set(context, &inData);
        if (ret)
        {
            std::cout << "aml_module_input_set fail." << std::endl;
            return -1;
        }
    }
    else if (input_mode == 1)
    {
        inData.input_type = BINARY_RAW_DATA;
        // do some preprocess
        tmpdata = (unsigned char *)malloc(input_size);
        for (int i = 0; i < 3; i++)
        {
            offset = input_width * input_height * i;
            for (int j = 0; j < input_width * input_height; j++)
            {
                tmpdata[j + offset] = rawdata[j * 3 + i];
            }
        }
        inData.input = tmpdata;
        ret = aml_module_input_set(context, &inData);
        if (ret)
        {
            std::cout << "aml_module_input_set fail." << std::endl;
            return -1;
        }
    }
    else if (input_mode == 2)
    {
        // do some preprocess
        tmpdata = (unsigned char *)malloc(input_size);
        for (int i = 0; i < 3; i++)
        {
            offset = input_width * input_height * i;
            for (int j = 0; j < input_width * input_height; j++)
            {
                tmpdata[j + offset] = rawdata[j * 3 + i];
            }
        }
        memcpy(inbuf, tmpdata, insize);
        aml_util_flushTensorHandle(context, AML_INPUT_TENSOR);  //!!note,flush the input buffer in dma mode
    }
    else
    {
        std::cout << "input_mode error, please check it." << std::endl;
        return -1;
    }

    if (tmpdata)
    {
        free(tmpdata);
        tmpdata = NULL;
    }
    if (rawdata)
    {
        free(rawdata);
        rawdata = NULL;
    }

    /* inference & getoutput & postprocess */
    memset(&outconfig,0,sizeof(aml_output_config_t));
    if (output_mode == 0)
    {
        outconfig.format = AML_OUTDATA_FLOAT32;
        outdata = (nn_output *)aml_module_output_get(context, outconfig);
        if (NULL == outdata)
        {
            std::cout << "aml_module_output_get fail." << std::endl;
            return -1;
        }
        yolov5_out = (yolov5_out_t *)postprocess_yolov5s(outdata);
    }
    else if (output_mode == 1)
    {
        outconfig.format = AML_OUTDATA_RAW;
        outdata = (nn_output *)aml_module_output_get(context, outconfig);
        if (NULL == outdata)
        {
            std::cout << "aml_module_output_get fail." << std::endl;
            return -1;
        }
        yolov5_out = (yolov5_out_t *)postprocess_yolov5s_dequant(outdata);
    }
    else if (output_mode == 2)
    {
        // outconfig.perfMode = AML_PERF_INFERENCE;
        // aml_module_output_get(context, outconfig);

        outconfig.format = AML_OUTDATA_DMA;
        outdata = (nn_output *)aml_module_output_get(context, outconfig);
        if (NULL == outdata)
        {
            std::cout << "aml_module_output_get fail." << std::endl;
            return -1;
        }
        yolov5_out = (yolov5_out_t *)postprocess_yolov5s_dequant(outdata);// you can also get output from outbuf[i], then do postprocess
    }

    std::cout << "detNum = " << yolov5_out->detNum << std::endl;
    for (int i = 0; i < yolov5_out->detNum; i++)
    {
        std::cout << "obj_number: " << i
                  << ", class_id = " << int(yolov5_out->pBox[i].objectClass)
                  << ", class_name = " << coco_names[int(yolov5_out->pBox[i].objectClass)]
                  << ", left = " << yolov5_out->pBox[i].x - yolov5_out->pBox[i].w / 2
                  << ", top = " << yolov5_out->pBox[i].y - yolov5_out->pBox[i].h / 2
                  << ", right = " << yolov5_out->pBox[i].x + yolov5_out->pBox[i].w / 2
                  << ", bot = " << yolov5_out->pBox[i].y + yolov5_out->pBox[i].h / 2
                  << ", prob = " << yolov5_out->pBox[i].score
                  << std::endl;
    }

    if (input_mode == 2)
    {
        if (inbuf)
        {
            aml_util_freeAlignedBuffer(NULL, inbuf);
        }
    }
    if (output_mode == 2)
    {
        for (int i = 0; i < output_tensor->num; i++)
        {
            if (outbuf[i])
            {
                aml_util_freeAlignedBuffer(NULL, outbuf[i]);
            }
        }
    }

    /* free tensor info */
    ret = aml_util_freeTensorInfo(input_tensor);
    if (ret)
    {
        std::cout << "aml_util_freeTensorInfo fail." << std::endl;
        return -1;
    }
    ret = aml_util_freeTensorInfo(output_tensor);
    if (ret)
    {
        std::cout << "aml_util_freeTensorInfo fail." << std::endl;
        return -1;
    }

    /* model release */
    ret = aml_module_destroy(context);
    if (ret)
    {
        std::cout << "aml_module_destroy fail." << std::endl;
        return -1;
    }

    return ret;
}