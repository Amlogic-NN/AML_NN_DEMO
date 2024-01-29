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
#include <math.h>
#include <unistd.h>
#include "nn_sdk.h"
#include "nn_util.h"
#include "nn_demo.h"
#include <pthread.h>
#include "jpeglib.h"

void* postprocess_facedet(nn_output *pout);

///////////////////////////////////////////////////////////
#define BILLION 1000000000
static uint64_t tmsStart, tmsEnd, msVal_inference, usVal_inference;
static uint64_t get_perf_count()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)((uint64_t)ts.tv_nsec + (uint64_t)ts.tv_sec * BILLION);
}

static void *read_file(const char *file_path, int *file_size)
{
    FILE *fp = NULL;
    int size = 0;
    void *buf = NULL;
    fp = fopen(file_path, "rb");
    if (NULL == fp)
    {
        printf("open file fail!\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    buf = malloc(sizeof(unsigned char) * size);

    fread(buf, 1, size, fp);

    fclose(fp);

    *file_size = size;
    return buf;
}

void* init_network_file(char **argv)
{
    void *qcontext = NULL;
    aml_config config;
    memset(&config, 0, sizeof(aml_config));

    config.nbgType = NN_ADLA_FILE;
    config.path = (const char *)argv[1];
    config.modelType = ADLA_LOADABLE;
    config.typeSize = sizeof(aml_config);

    qcontext = aml_module_create(&config);
    if (qcontext == NULL)
    {
        printf("aml_module_create fail.\n");
        return NULL;
    }

    if (config.nbgType == NN_ADLA_MEMORY && config.pdata != NULL)
    {
        free((void*)config.pdata);
    }
    return qcontext;
}

int run_network(void *qcontext, char **argv)
{
    int ret = 0;
    nn_input inData;
    char *input_path = NULL;
    int input_size = 0;
    unsigned char *rawdata = NULL;
    nn_output *outdata = NULL;
    aml_output_config_t outconfig;

    face_detect_out_t *face_detect_out = NULL;
    float prob = 0;
    unsigned int i = 0, j = 0, input_width = 0, input_high = 0;

    input_path = argv[2];
    rawdata = read_file(input_path, &input_size);
    inData.input = rawdata;
    inData.input_index = 0;
    inData.size = input_size;
    inData.input_type = BINARY_RAW_DATA;

    ret = aml_module_input_set(qcontext, &inData);
    if (ret)
    {
        printf("aml_module_input_set fail.\n");
        return -1;
    }

    memset(&outconfig,0,sizeof(aml_output_config_t));
    outconfig.format = AML_OUTDATA_FLOAT32;
    outconfig.typeSize = sizeof(aml_output_config_t);
    outdata = (nn_output*)aml_module_output_get(qcontext, outconfig);
    if (outdata == NULL)
    {
        printf("aml_module_output_get fail.\n");
        return -1;
    }

    face_detect_out = postprocess_facedet(outdata);
    printf("face_detNum = %d\n", face_detect_out->detNum);
    input_width = 640, input_high = 384;
    for (i = 0; i < face_detect_out->detNum; i++)
    {
        prob = face_detect_out->pBox[i].score;
        printf("face_number:%d, x:%f, y:%f, w:%f, h:%f, prob:%f\n", i, face_detect_out->pBox[i].x*input_width, face_detect_out->pBox[i].y*input_high, face_detect_out->pBox[i].w*input_width, face_detect_out->pBox[i].h*input_high, prob);
    }

    return ret;
}

int destroy_network(void *qcontext)
{
    int ret = 0;
    ret = aml_module_destroy(qcontext);
    if (ret)
    {
        printf("aml_module_destroy fail.\n");
        return -1;
    }
    return ret;
}


int main(int argc,char **argv)
{
	if (argc < 3)
	{
        printf("Usage:\n");
        printf("argv[0]: exe_file\n");
        printf("argv[1]: xxx.adla\n");
        printf("argv[2]: xxx.bin\n");
		return -1;
	}

    int ret = 0;
    void *context = NULL;

    context = init_network_file(argv);
    if (context == NULL)
    {
        printf("init_network fail.\n");
        return -1;
    }

    ret = run_network(context, argv);
    if (ret != 0)
    {
        printf("run_network fail.\n");
        return -1;
    }

    ret = destroy_network(context);
    if (ret != 0)
    {
        printf("destroy_network fail.\n");
        return -1;
    }

    return ret;
}