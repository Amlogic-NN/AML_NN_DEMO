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
#include "head_detection.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"
#include "jpeglib.h"
/*******************     face detect postprocess      ********************/
/****************************************************************************/

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
                continue;
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

float sigmod(float x)
{
    return 1.0/(1+exp(-x));
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

float box_iou(box a, box b)
{
    return box_intersection(a, b)/box_union(a, b);
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


/*******************     head detect postprocess      ********************/
/****************************************************************************/
void* postprocess_headdet(nn_output *pout)
{
    float *buffer;
    int i,j,m,index;
    unsigned int sz;
    float result_buffer[13][13][5][6];
    float buffer4th[13][13][5];//buffer4th.shape is (13,13,5)
    float buffer5th[13][13][5];//buffer5th.shape is (13,13,5,1)
    float max,min,classes,confidence;
    float obj_threshold = 0.3;
    float nms_threshold = 0.3;
    float x,y,w,h;
    float anchors[10] = {0.57273, 0.677385, 1.87446, 2.06253, 3.33843, 5.47434, 7.88282, 3.52778, 9.77052, 9.16828};
    box *pboxes;
    int box_num = 0,det_num = 0;
    float ** probs = NULL;
    head_det_out_t *headdet_result = NULL;
    detBox *det_boxes = NULL;

    headdet_result = (head_det_out_t*)malloc(sizeof(head_det_out_t));

    buffer = (float*)pout->out[0].buf;
    sz= pout->out[0].size;
    memcpy(result_buffer,buffer,sz);

    /*get the 4th dimension of result_buffer,and sigmod the array*/
    for (i = 0;i < 13;i++)
    {
        for (j = 0;j < 13;j++)
        {
            for (m = 0;m < 5;m++)
            {
                buffer4th[i][j][m] = sigmod(result_buffer[i][j][m][4]);
            }
        }
    }
    /*get the 5th dimension of result_buffer,get its max and min element*/
    for (i = 0;i < 13;i++)
    {
        for (j = 0;j < 13;j++)
        {
            for (m = 0;m < 5;m++)
            {
                buffer5th[i][j][m] = result_buffer[i][j][m][5];
            }
        }
    }

    max = buffer5th[0][0][0];
    min = buffer5th[0][0][0];
    for (i = 0;i < 13;i++)
    {
        for (j = 0;j < 13;j++)
        {
            for (m = 0;m < 5;m++)
            {
                if (buffer5th[i][j][m] > max)
                {
                    max = buffer5th[i][j][m];
                }
                if (buffer5th[i][j][m] < min)
                {
                    min = buffer5th[i][j][m];
                }
            }
        }
    }
    /*softmax(buffer5th)*/
    for (i = 0;i < 13;i++)
    {
        for (j = 0;j < 13;j++)
        {
            for (m = 0;m < 5;m++)
            {
                buffer5th[i][j][m] -= max;
            }
        }
    }

    if (min < (-100.0))
    {
        for (i = 0;i < 13;i++)
        {
            for (j = 0;j < 13;j++)
            {
                for (m = 0;m < 5;m++)
                {
                    buffer5th[i][j][m] = buffer5th[i][j][m]/(min*(-100.0));
                }
            }
        }
    }
    /*
    1. e_x = np.exp(x)
    2. e_x / e_x.sum(axis, keepdims=True) ;[...,1.0,...]
       As e_x.sum(axis, keepdims=True) = e_x  {because e_x.shape is (13,13,5,1)};
       so  e_x / e_x.sum(axis, keepdims=True) must be [...,1.0,...]
    */
    for (i = 0;i < 13;i++)
    {
        for (j = 0;j < 13;j++)
        {
            for (m = 0;m < 5;m++)
            {
                buffer5th[i][j][m] = exp(buffer5th[i][j][m]);
                buffer5th[i][j][m] = 1.0;
            }
        }
    }
    /*
    netout[..., 5:] = netout[..., 4][..., np.newaxis] * _softmax(netout[..., 5:])
    so buffer5th = buffer4th;
    */
    for (i = 0;i < 13;i++)
    {
        for (j = 0;j < 13;j++)
        {
            for (m = 0;m < 5;m++)
            {
                buffer5th[i][j][m] = buffer5th[i][j][m] * buffer4th[i][j][m];
            }
        }
    }
    /* netout[..., 5:] *= netout[..., 5:] > obj_threshold */
    for (i = 0;i < 13;i++)
    {
        for (j = 0;j < 13;j++)
        {
            for (m = 0;m < 5;m++)
            {
                if (buffer5th[i][j][m] < obj_threshold)
                    buffer5th[i][j][m] = 0.0;
                else
                    box_num += 1;
            }
        }
    }

    pboxes = (box*)malloc(sizeof(box)*box_num);
    probs = (float**)calloc(box_num,sizeof(float*)*2);

    index = 0;
    for (i = 0;i < 13;i++)
    {
        for (j = 0;j < 13;j++)
        {
            for (m = 0;m < 5;m++)
            {
                classes = buffer5th[i][j][m];
                if (classes > 0.0)
                {
                    /* reseult_buffer fist 4 elemnt are x,y,w,h */
                    x = (float)result_buffer[i][j][m][0];
                    y = (float)result_buffer[i][j][m][1];
                    w = (float)result_buffer[i][j][m][2];
                    h = (float)result_buffer[i][j][m][3];

                    x = (j + sigmod(x)) / 13;
                    y = (i + sigmod(y)) / 13;
                    w = anchors[m*2 +0] * exp(w) / 13;
                    h = anchors[m*2 +1] * exp(h) / 13;

                    confidence = buffer4th[i][j][m];

                    if (index <= box_num)
                    {
                        pboxes[index].x = x ;
                        pboxes[index].y = y;
                        pboxes[index].w = w;
                        pboxes[index].h = h;

                        probs[index] = (float*)calloc(2,sizeof(float));
                        probs[index][0] = classes;
                        probs[index][1] = confidence;
                    }
                    else
                    {
                        printf("[post_process] The number of boxes exceeds\n");
                    }
                    index += 1;
                }
            }
        }
    }

    do_nms_sort(pboxes,probs,box_num,1,nms_threshold);
    for (i = 0;i < box_num;i++)
        if (probs[i][0] > 0)det_num += 1;

    det_boxes = (detBox *)malloc(det_num * sizeof(detBox));
    index = 0;
    for (i = 0;i < box_num;i++)
    {
        if (probs[i][0] > 0)
        {
            det_boxes[index].x = pboxes[i].x;
            det_boxes[index].y = pboxes[i].y;
            det_boxes[index].w = pboxes[i].w;
            det_boxes[index].h = pboxes[i].h;
            det_boxes[index].score = probs[i][0];
            det_boxes[index].objectClass = probs[i][1];
            index += 1;
        }
    }
    headdet_result->headOut.detNum = det_num;
    headdet_result->headOut.pBox = det_boxes;
    for (i = 0;i < box_num;i++)
        if (probs[i])free(probs[i]);
    if (probs)free(probs);
    if (pboxes)free(pboxes);
    return (void*)headdet_result;
}


/****************************************************************************/
/*******************     head detect postprocess       ********************/



