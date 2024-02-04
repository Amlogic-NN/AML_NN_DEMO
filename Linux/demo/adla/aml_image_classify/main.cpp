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
    aml_memory_config_t mem_config;
    aml_memory_data_t memory_data;

    nn_output *outdata = NULL;
    aml_output_config_t outconfig;

    tensor_info *input_tensor;
    tensor_info *output_tensor;

    img_classify_out_t *cls_out = NULL;

    model_path = (const char *)argv[1];
    input_path = (const char *)argv[2];
    input_mode = std::stoi(argv[3]);
    output_mode = std::stoi(argv[4]);

    input_width = 224;
    input_height = 224;
    input_size = input_width * input_height * 3;

    /* model init */
    memset(&config, 0, sizeof(aml_config));
    config.nbgType = NN_ADLA_FILE;
    config.path = model_path;
    config.modelType = ADLA_LOADABLE;
    config.typeSize = sizeof(aml_config);
    context = aml_module_create(&config);
    if (NULL == context)
    {
        std::cout << "aml_module_create fail." << std::endl;
        return -1;
    }

    /* get tensor info */
    ret = aml_util_getTensorInfo(context, NULL, &input_tensor, &output_tensor);
    if (ret)
    {
        std::cout << "aml_util_getTensorInfo fail." << std::endl;
        return -1;
    }
    std::cout << "input tensors: " << input_tensor->num << std::endl;
    std::cout << "output tensors: " << output_tensor->num << std::endl;

    std::cout << "input_" << 0 << " info: "
              << "n: " << input_tensor->info[0].sizes_of_dim[0] << " "
              << "h: " << input_tensor->info[0].sizes_of_dim[1] << " "
              << "w: " << input_tensor->info[0].sizes_of_dim[2] << " "
              << "c: " << input_tensor->info[0].sizes_of_dim[3] << " "
              << "zp: " << input_tensor->info[0].TF_zeropoint << " "
              << "scale: " << input_tensor->info[0].TF_scale << " "
              << "format: " << input_tensor->info[0].data_format << " "
              << std::endl;

    for (int i = 0; i < output_tensor->num; i++)
    {
        std::cout << "output_" << i << " info: "
                  << "n: " << output_tensor->info[i].sizes_of_dim[0] << " "
                  << "h: " << output_tensor->info[i].sizes_of_dim[1] << " "
                  << "w: " << output_tensor->info[i].sizes_of_dim[2] << " "
                  << "c: " << output_tensor->info[i].sizes_of_dim[3] << " "
                  << "zp: " << output_tensor->info[i].TF_zeropoint << " "
                  << "scale: " << output_tensor->info[i].TF_scale << " "
                  << "format: " << output_tensor->info[i].data_format << " "
                  << std::endl;
    }

    if (input_mode == 2)
    {
        mem_config.cache_type = AML_WITH_CACHE;
        mem_config.memory_type = AML_VIRTUAL_ADDR;
        mem_config.direction = AML_MEM_DIRECTION_READ_WRITE;
        mem_config.index = 0;
        mem_config.mem_size = input_size;
        ret = aml_util_mallocBuffer(context, &mem_config, &memory_data);
        if (ret)
        {
            std::cout << "aml_util_mallocBuffer fail." << std::endl;
            return -1;
        }
    }

    /* setinput */
    rawdata = get_jpeg_rawData(input_path, input_width, input_height);
    if (input_mode == 0)
    {
        inData.input_type = RGB24_RAW_DATA;
        inData.input = rawdata;
    }
    else if (input_mode == 1)
    {
        inData.input_type = BINARY_RAW_DATA;
        // do some preprocess
        for (int i = 0; i < input_size; i++)
        {
            rawdata[i] = rawdata[i] - 128;
        }
        inData.input = rawdata;
    }
    else if (input_mode == 2)
    {
        inData.input_type = INPUT_DMA_DATA;
        // do some preprocess
        for (int i = 0; i < input_size; i++)
        {
            rawdata[i] = rawdata[i] - 128;
        }
        memcpy(memory_data.viraddr, rawdata, mem_config.mem_size);
        inData.input = NULL;
    }
    else
    {
        std::cout << "input_mode error, please check it." << std::endl;
        return -1;
    }

    inData.input_index = 0;
    inData.size = input_size;
    ret = aml_module_input_set(context, &inData);
    if (ret)
    {
        std::cout << "aml_module_input_set fail." << std::endl;
        return -1;
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
        cls_out = (img_classify_out_t *)process_top5_f32((float *)outdata->out[0].buf, outdata->out[0].size/sizeof(float));
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
        cls_out = (img_classify_out_t *)process_top5_i8((int8_t *)outdata->out[0].buf, outdata->out[0].size);
    }
    else if (output_mode == 2)
    {
        outconfig.format = AML_OUTDATA_DMA;
        outdata = (nn_output *)aml_module_output_get(context, outconfig);
        if (NULL == outdata)
        {
            std::cout << "aml_module_output_get fail." << std::endl;
            return -1;
        }
        cls_out = (img_classify_out_t *)process_top5_i8((int8_t *)outdata->out[0].buf, outdata->out[0].size);
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

    if (input_mode == 2)
    {
        ret = aml_util_freeBuffer(context, &mem_config, &memory_data);
        if (ret)
        {
            std::cout << "aml_util_freeBuffer fail." << std::endl;
            return -1;
        }
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