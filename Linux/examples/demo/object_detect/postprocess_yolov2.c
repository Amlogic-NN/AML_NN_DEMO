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
#include <math.h>
#include "nn_util.h"
#include "nn_sdk.h"
#include "nn_demo.h"
#define FLT_MAX 3.402823466e+38F
int g_detect_number = 50;

box get_region_box(float *x, float *biases, int n, int index, int i, int j, int w, int h)
{
    box b;

    b.x = (i + logistic_activate(x[index + 0])) / w;
    b.y = (j + logistic_activate(x[index + 1])) / h;
    b.w = exp(x[index + 2]) * biases[2*n]   / w;
    b.h = exp(x[index + 3]) * biases[2*n+1] / h;
    return b;
}


int max_index(float *a, int n)
{
    int i, max_i = 0;
    float max = a[0];

    if (n <= 0)
        return -1;

    for (i = 1; i < n; ++i)
    {
        if (a[i] > max)
        {
            max = a[i];
            max_i = i;
        }
    }
    return max_i;
}

float overlap(float x1, float w1, float x2, float w2)
{
    float l1 = x1 - w1/2;
    float l2 = x2 - w2/2;
    float left = l1 > l2 ? l1 : l2;
    float r1 = x1 + w1/2;
    float r2 = x2 + w2/2;
    float right = r1 < r2 ? r1 : r2;
    return right - left;
}

float box_intersection(box a, box b)
{
    float area = 0;
    float w = overlap(a.x, a.w, b.x, b.w);
    float h = overlap(a.y, a.h, b.y, b.h);
    if (w < 0 || h < 0)
        return 0;
    area = w*h;
    return area;
}

float box_union(box a, box b)
{
    float i = box_intersection(a, b);
    float u = a.w*a.h + b.w*b.h - i;
    return u;
}

float box_iou(box a, box b)
{
    return box_intersection(a, b)/box_union(a, b);
}

int nms_comparator(const void *pa, const void *pb)
{
    sortable_bbox a = *(sortable_bbox *)pa;
    sortable_bbox b = *(sortable_bbox *)pb;
    float diff = a.probs[a.index][b.classId] - b.probs[b.index][b.classId];
    if (diff < 0) return 1;
    else if(diff > 0) return -1;
    return 0;
}

void do_nms_sort(box *boxes, float **probs, int total, int classes, float thresh)
{
    int i, j, k;
    sortable_bbox *s = (sortable_bbox *)calloc(total, sizeof(sortable_bbox));

    for (i = 0; i < total; ++i)
    {
        s[i].index = i;
        s[i].classId = 0;
        s[i].probs = probs;
    }

    for (k = 0; k < classes; ++k)
    {
        for (i = 0; i < total; ++i)
        {
            s[i].classId = k;
        }
        qsort(s, total, sizeof(sortable_bbox), nms_comparator);
        for (i = 0; i < total; ++i)
        {
            if (probs[s[i].index][k] == 0)
            {
                continue;
            }

            for (j = i+1; j < total; ++j)
            {
                box b = boxes[s[j].index];
                if (probs[s[j].index][k]>0)
                {
                    if (box_iou(boxes[s[i].index], b) > thresh)
                    {
                        probs[s[j].index][k] = 0;
                    }
                }
            }
        }
    }
    free(s);
}

void flatten(float *x, int size, int layers, int batch, int forward)
{
    float *swap = (float*)calloc(size*layers*batch, sizeof(float));
    int i,c,b;
    for (b = 0; b < batch; ++b)
    {
        for (c = 0; c < layers; ++c)
        {
            for (i = 0; i < size; ++i)
            {
                int i1 = b*layers*size + c*size + i;
                int i2 = b*layers*size + i*layers + c;
                if (forward) swap[i2] = x[i1];
                else swap[i1] = x[i2];
            }
        }
    }
    memcpy(x, swap, size*layers*batch*sizeof(float));
    free(swap);
}

void softmax(float *input, int n, float temp, float *output)
{
    int i;
    float sum = 0;
    float largest = -FLT_MAX;
    for (i = 0; i < n; ++i)
    {
        if (input[i] > largest) largest = input[i];
    }
    for (i = 0; i < n; ++i)
    {
        float e = exp(input[i]/temp - largest/temp);
        sum += e;
        output[i] = e;
    }
    for (i = 0; i < n; ++i)
    {
        output[i] /= sum;
    }
}

float sigmod(float x)
{
    return 1.0/(1+exp(-x));
}

float logistic_activate(float x)
{
    return 1./(1. + exp(-x));
}


unsigned char *transpose(const unsigned char * src,int width,int height)
{
    unsigned char* dst;
    int i,j,m;
    int channel = 3;

    dst = (unsigned char*)malloc(width*height*channel);
    memset(dst,0,width*height*channel);

    /*hwc -> whc*/
    for (i = 0;i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            for (m = 0;m < channel;m++)
                *(dst + i * height * channel + j * channel + m) = *(src + j * width * channel + i * channel + m);
        }
    }
    return dst;
}

int entry_index(int lw, int lh, int lclasses, int loutputs, int batch, int location, int entry)
{
    int n = location / (lw*lh);
    int loc = location % (lw*lh);
    return batch * loutputs + n * lw*lh*(4 + lclasses + 1) + entry * lw*lh + loc;
}

void activate_array(float *start, int num)
{
    for (int i = 0; i < num; i ++){
        start[i] = logistic_activate(start[i]);
    }
}

obj_detect_out_t dectout ;
void* yolov2_result(int num, float thresh, box *boxes, float **probs, int classes)
{
    int i=0,detect_num = 0;

    if (dectout.pBox == NULL)
    {
        dectout.pBox = (detBox*)malloc(MAX_DETECT_NUM*sizeof(detBox));
    }
    if (dectout.pBox == NULL)
    {
        return NULL;
    }
    for (i = 0; i < num; ++i)
    {
        int classId = max_index(probs[i], classes);
        float prob = probs[i][classId];
        if (prob > thresh)
        {
            if (detect_num >= g_detect_number)
            {
                break;
            }
            dectout.pBox[detect_num].x = boxes[i].x;
            dectout.pBox[detect_num].y = boxes[i].y;
            dectout.pBox[detect_num].w = boxes[i].w;
            dectout.pBox[detect_num].h = boxes[i].h;
            dectout.pBox[detect_num].score = prob;
            dectout.pBox[detect_num].objectClass = (float)classId;
            detect_num++ ;
        }
    }
    dectout.detNum = detect_num;
    return (void*)&dectout;
}


void* object_detect_postprocess(float *predictions, int width, int height, int modelWidth, int modelHeight, int input_num)
{
    int i,j,n;
    float threshold = 0.24;
    float iou_threshold = 0.4;
    int num_class = 80;
    int num_box = 5;
    int grid_size = 13;
    float biases[10] = {0.738768,0.874946,2.422040,2.657040,4.309710,7.044930,10.246000,4.594280,12.686800,11.874100};
    void* objout = NULL;

    box *boxes = (box *)calloc(modelWidth*modelHeight*num_box, sizeof(box));
    float **probs = (float **)calloc(modelWidth*modelHeight*num_box, sizeof(float *));

    for (j = 0; j < modelWidth*modelHeight*num_box; ++j)
    {
        probs[j] = (float *)calloc(num_class+1, sizeof(float *));// calloc "num_class+1" float for every W*H*num_box
    }

    {
        int i,b;
        int coords = 4,classes = 80;
        int size = coords + classes + 1;
        int w = 13;
        int h = 13;
        int n = 425/size;
        int batch = 1;
        flatten(predictions, w*h, size*n, batch, 1);

        for (b = 0; b < batch; ++b)
        {
            for (i = 0; i < h*w*n; ++i)
            {
                int index = size*i + b*input_num;
                predictions[index + 4] = logistic_activate(predictions[index + 4]);
            }
        }

        for (b = 0; b < batch; ++b)
        {
            for (i = 0; i < h*w*n; ++i)
            {
                int index = size*i + b*input_num;
                softmax(predictions + index + 5, classes, 1, predictions + index + 5);
            }
        }
    }

    for (i = 0; i < modelWidth*modelHeight; ++i)
    {
        int row = i / modelWidth;
        int col = i % modelWidth;
        for (n = 0; n < num_box; ++n)
        {
            int index = i*num_box + n;
            int p_index = index * (num_class + 5) + 4;
            float scale = predictions[p_index];
            int box_index = index * (num_class + 5);
            int class_index = 0;
            boxes[index] = get_region_box(predictions, biases, n, box_index, col, row, modelWidth, modelHeight);
            class_index = index * (num_class + 5) + 5;
            for (j = 0; j < num_class; ++j)
            {
                float prob = scale*predictions[class_index+j];
                probs[index][j] = (prob > threshold) ? prob : 0;
            }
        }
    }

    do_nms_sort(boxes, probs, grid_size*grid_size*num_box, num_class, iou_threshold);
    objout = yolov2_result(grid_size*grid_size*num_box, threshold, boxes, probs, num_class);

    free(boxes);
    boxes = NULL;

    for (j = 0; j < grid_size*grid_size*num_box; ++j) {
        free(probs[j]);
        probs[j] = NULL;
    }

    free(probs);
    probs = NULL;
    return objout;
}

void* postprocess_object_detect(nn_output *pout)
{
    float* obj_out;
    int output_num;

    obj_out = (float*)pout->out[0].buf;
    output_num = pout->out[0].size/sizeof(float);

    return object_detect_postprocess(obj_out,416,416,13,13,output_num);
}

void *post_process_all_module(aml_module_t type,nn_output *pOut)
{
	aml_module_t nettype = type;
	void *data = NULL;

	switch (nettype)
	{
	case OBJECT_DETECT:
        data  = postprocess_object_detect(pOut);
		break;
	default:
		break;
	}
	return data;
}
