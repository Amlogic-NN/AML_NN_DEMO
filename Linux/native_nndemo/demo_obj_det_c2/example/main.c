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

void* postprocess_object_detect(nn_output *pout);

static const char *coco_names[] = {"person","bicycle","car","motorbike","airplane","bus","train",
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

    config.nbgType = NN_NBG_FILE;
    config.path = (const char *)argv[1];
    config.modelType = DARKNET;
    config.typeSize = sizeof(aml_config);

    qcontext = aml_module_create(&config);
    if (qcontext == NULL)
    {
        printf("aml_module_create fail.\n");
        return NULL;
    }

    if (config.nbgType == NN_NBG_MEMORY && config.pdata != NULL)
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

    obj_detect_out_t *object_detect_out = NULL;
    int classid = 0;
    float prob = 0;
    float cx,cy,w,h;
    float left = 0, right = 0, top = 0, bot = 0;

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

    object_detect_out = (obj_detect_out_t *)postprocess_object_detect(outdata);
    // for (int i =0;i < object_detect_out->detNum;i++)
    // {
    //     classid = (int)object_detect_out->pBox[i].objectClass;
    //     prob = object_detect_out->pBox[i].score;

    //     cx = object_detect_out->pBox[i].x*416;
    //     cy = object_detect_out->pBox[i].y*416;
    //     w = object_detect_out->pBox[i].w*416;
    //     h = object_detect_out->pBox[i].h*416;

    //     printf("class:%s, label_num:%d, prob:%f, cx:%f, cy:%f, w:%f, h:%f\n", coco_names[classid], classid, prob, cx, cy, w, h);
    // }
    for (int i = 0;i < object_detect_out->detNum;i++)
    {
        classid = (int)object_detect_out->pBox[i].objectClass;
        prob = object_detect_out->pBox[i].score;
        left  = (object_detect_out->pBox[i].x-object_detect_out->pBox[i].w/2.)*416;
        right = (object_detect_out->pBox[i].x+object_detect_out->pBox[i].w/2.)*416;
        top   = (object_detect_out->pBox[i].y-object_detect_out->pBox[i].h/2.)*416;
        bot   = (object_detect_out->pBox[i].y+object_detect_out->pBox[i].h/2.)*416;
        printf("obj_number:%d,class:%s,label num:%d,prob:%f\n",i,coco_names[classid],classid,prob);
        printf("left:%f,right:%f,top:%f,bot:%f\n\n",left,right,top,bot);
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