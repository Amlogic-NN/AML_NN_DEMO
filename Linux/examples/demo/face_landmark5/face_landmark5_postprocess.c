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
#include "face_landmark5.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"
#include "jpeglib.h"

#define X_MAX 639.0 / 640.0
#define Y_MAX 383.0 / 384.0

typedef struct
{
    int index;
    int classId;
    float probs;
} sortable_bbox_retina;

float bbox_32[12][20][8]; //384/32,640/32
float bbox_16[24][40][8];
float bbox_8[48][80][8];
//prob score,input
float prob_32[240][2][2];
float prob_16[960][2][2];
float prob_8[3840][2][2];
//land mark
float land_32[12][20][20]; //384/32,640/32
float land_16[24][40][20];
float land_8[48][80][20];

static float prob32[480][1];
static float prob16[1920][1];
static float prob8[7680][1];
//output box
static box box32[12][20][2];
static box *pbox32;
static box box16[24][40][2];
static box *pbox16;
static box box8[48][80][2];
static box *pbox8;
//landmark
static landmark land32[12][20][2][5];
static landmark *pland32;
static landmark land16[24][40][2][5];
static landmark *pland16;
static landmark land8[48][80][2][5];
static landmark *pland8;

int g_detect_number = 230;

/*******************     postprocess util      ********************/
/****************************************************************************/

float retina_overlap(float x1, float w1, float x2, float w2)
{
    float l1 = x1;
    float l2 = x2;
    float left = l1 > l2 ? l1 : l2;
    float r1 = x1 + w1;
    float r2 = x2 + w2;
    float right = r1 < r2 ? r1 : r2;
    return right - left;
}
float retina_box_intersection(box a, box b)
{
    float area = 0;
    float w = retina_overlap(a.x, a.w, b.x, b.w);
    float h = retina_overlap(a.y, a.h, b.y, b.h);
    if (w < 0 || h < 0)
        return 0;
    area = w*h;
    return area;
}
float retina_box_union(box a, box b)
{
    float i = retina_box_intersection(a, b);
    float u = a.w*a.h + b.w*b.h - i;
    return u;
}
float retina_box_iou(box a, box b)
{
    return retina_box_intersection(a, b)/retina_box_union(a, b);
}

int retina_nms_comparator(const void *pa, const void *pb)
{
    sortable_bbox_retina a = *(sortable_bbox_retina *)pa;
    sortable_bbox_retina b = *(sortable_bbox_retina *)pb;
    float diff = a.probs - b.probs;
    if (diff < 0) return 1;
    else if (diff > 0) return -1;
    return 0;
}

void do_global_sort(box *boxe1,box *boxe2, float prob1[][1],float prob2[][1], int len1,int len2,float thresh)
{
    int i,j;
    for (i = 0; i < len1; ++i)
    {
        if (prob1[i][0] > thresh)
        {
            for (j = 0;j < len2;j++)
            {
                if (prob2[j][0] > thresh)
                {
                    if (retina_box_iou(boxe1[i], boxe2[j]) > 0.1)
                    {
                        if (prob2[j][0] > prob1[i][0])
                        {
                            prob1[i][0] = 0;
                        }
                        else
                        {
                            prob2[j][0] = 0;
                        }
                    }
                }
            }
        }
    }
}

void retina_do_nms_sort(box *boxes, float probs[][1], int total,  float thresh)
{
    int i, j;
    sortable_bbox_retina *s = (sortable_bbox_retina *)calloc(total, sizeof(sortable_bbox_retina));
    for (i = 0; i < total; ++i)
    {
        s[i].index = i;
        s[i].classId = 0;
        s[i].probs = probs[i][0];
        //printf("%f\n",s[i].probs);
    }

    for (i = 0; i < total; ++i)
    {
        s[i].classId = 0;
    }
    qsort(s, total, sizeof(sortable_bbox_retina), retina_nms_comparator);

    for (i = 0; i < total; ++i)
    {
        if (probs[s[i].index][0] >= 0.01)  //zxw
        {
            for (j = i+1; j < total; j++)
            {
                if (probs[s[j].index][0] >= 0.01)
                {
                    box b = boxes[s[j].index];
                    if (retina_box_iou(boxes[s[i].index], b) > thresh)
                    {
                        probs[s[j].index][0] = 0;
                    }
                }
            }
        }
    }
    free(s);
}

void retina_point5_result(int num, float thresh, box *boxes, float probs[][1],landmark *pland,face_landmark5_out_t* pface_landmark5_result)
{

    int i,j;
    int detect_num = pface_landmark5_result->detNum;
    for (i = 0; i < num; i++)
    {
        float prob = probs[i][0];
        if (detect_num < MAX_DETECT_NUM)
        {
            if (prob > thresh)
            {
                if (detect_num >= g_detect_number)
                {
                    break;
                }
                int left = 0;
                int right = 0;
                int top = 0;
                int bot = 0;

                left  = boxes[i].x / 640.0;
                right = (boxes[i].x + boxes[i].w) / 640.0;
                top   = boxes[i].y / 384.0;
                bot   = (boxes[i].y + boxes[i].h) / 384.0;

                if (left < 0) left = 0;
                if (right > 1) right = 1.0;
                if (top < 0) top = 0;
                if (bot > 1) bot = 1.0;
                pface_landmark5_result->facebox[detect_num].x = boxes[i].x / 640.0;
                pface_landmark5_result->facebox[detect_num].y = boxes[i].y / 384.0;
                pface_landmark5_result->facebox[detect_num].w = boxes[i].w / 640.0;
                pface_landmark5_result->facebox[detect_num].h = boxes[i].h / 384.0;
                if (pface_landmark5_result->facebox[detect_num].x <= 0 ) pface_landmark5_result->facebox[detect_num].x =0.000001;
                if (pface_landmark5_result->facebox[detect_num].y <= 0 ) pface_landmark5_result->facebox[detect_num].y =0.000001;
                if (pface_landmark5_result->facebox[detect_num].w <= 0 ) pface_landmark5_result->facebox[detect_num].w =0.000001;
                if (pface_landmark5_result->facebox[detect_num].h <= 0 ) pface_landmark5_result->facebox[detect_num].h =0.000001;
                if (pface_landmark5_result->facebox[detect_num].x >= 1 ) pface_landmark5_result->facebox[detect_num].x =0.999999;
                if (pface_landmark5_result->facebox[detect_num].y >= 1 ) pface_landmark5_result->facebox[detect_num].y =0.999999;
                if (pface_landmark5_result->facebox[detect_num].w >= 1 ) pface_landmark5_result->facebox[detect_num].w =0.999999;
                if (pface_landmark5_result->facebox[detect_num].h >= 1 ) pface_landmark5_result->facebox[detect_num].h =0.999999;
                for (j=0 ;j <5 ; j++)
                {
                    pface_landmark5_result->pos[detect_num][j].x = pland[i * 5 + j].x / 640.0;
                    pface_landmark5_result->pos[detect_num][j].y = pland[i * 5 + j].y / 384.0;
                    if (pface_landmark5_result->pos[detect_num][j].x <= 0) pface_landmark5_result->pos[detect_num][j].x=0.001;
                    if (pface_landmark5_result->pos[detect_num][j].x >= X_MAX) pface_landmark5_result->pos[detect_num][j].x=0.997;
                    if (pface_landmark5_result->pos[detect_num][j].y <= 0) pface_landmark5_result->pos[detect_num][j].y=0.001;
                    if (pface_landmark5_result->pos[detect_num][j].y >= Y_MAX) pface_landmark5_result->pos[detect_num][j].y=0.997;
                    //printf("point number =%d,rawData-X:%.5f,Y:%.5f\n" , j, pface_landmark5_result->pos[detect_num][j].x, pface_landmark5_result->pos[detect_num][j].y);
                }
                detect_num++;
            }
        }
    }
    //printf("detect number =%d \n", detect_num);
    pface_landmark5_result->detNum = detect_num;
}

/****************************************************************************/
/*******************     postprocess util      ********************/

/*******************     face detect postprocess      ********************/
/****************************************************************************/

int face_landmark5_postprocess(face_landmark5_out_t* pface_landmark5_result )
{
    int i = 0,j = 0,k = 0,x,y;
    int idx;
    int h32,w32,h16,w16,h8,w8;
    int ctrx,ctry,pred_ctrx,pred_ctry,predw,predh;
    int valid_8 = 0,valid_16 = 0,valid_32 = 0;

    pbox32 = (box*)box32;
    pbox16 = (box*)box16;
    pbox8 = (box*)box8;
    pland32 = (landmark *)land32;
    pland16 = (landmark *)land16;
    pland8 = (landmark *)land8;

    for (i=0;i<240;i++)
    {
        for (j=0;j<2;j++)
        {
             prob32[k][0] = prob_32[i][j][1];
             if (prob32[k][0] < 0.7)
             {
                 prob32[k][0] = 0;
             }
             else
             {
                 valid_32 = 1;
             }
             k++;
        }
    }
    k = 0;
    for (i=0;i<960;i++)
    {
        for (j=0;j<2;j++)
        {
             prob16[k][0] = prob_16[i][j][1];
             if (prob16[k][0] < 0.7)
             {
                 prob16[k][0] = 0;
             }
             else
             {
                 valid_16 = 1;
             }
             k++;
        }
    }
    k = 0;
    for (i=0;i<3840;i++)
    {
        for (j=0;j<2;j++)
        {
            prob8[k][0] = prob_8[i][j][1];
            if (prob8[k][0] < 0.7)
            {
                prob8[k][0] = 0;
            }
            else
            {
                valid_8 = 1;
            }
            k++;
        }
    }

    if (valid_32)
    {
        for (y=0;y<12;y++)
        {
            for (x=0;x<20;x++)
            {
                for (idx=0;idx<2;idx++)
                {
                    if (prob32[40*y+2*x+idx][0] <= 0.001)   //should not parse it if score too low
                        continue;
                    if (idx == 0)
                        h32=w32=512;
                    else
                        h32=w32=256;
                    ctrx = 32*x+7.5;
                    ctry = 32*y+7.5;
                    pred_ctrx = bbox_32[y][x][4*idx]+ctrx;
                    pred_ctry = bbox_32[y][x][4*idx+1]+ctry;
                    predw = exp(bbox_32[y][x][4*idx+2])*w32;
                    predh = exp(bbox_32[y][x][4*idx+3])*h32;
                    box32[y][x][idx].x = pred_ctrx-0.5*(predw-1);
                    box32[y][x][idx].y = pred_ctry-0.5*(predh-1);
                    box32[y][x][idx].w = predw;
                    box32[y][x][idx].h = predh;
                    for (i=0;i<5;i++)
                    {
                        land32[y][x][idx][i].x=land_32[y][x][10*idx+2*i]*w32+ctrx;
                        land32[y][x][idx][i].y=land_32[y][x][10*idx+2*i+1]*h32+ctry;
                    }

                }
            }
        }
    }

    if (valid_16)
    {
        for (y=0;y<24;y++)
        {
            for (x=0;x<40;x++)
            {
                for (idx=0;idx<2;idx++)
                {
                    if (prob16[80*y+2*x+idx][0] <= 0.001)   //should not parse it if score too low
                        continue;
                    if (idx == 0)
                        h16=w16=128;
                    else
                        h16=w16=64;
                    ctrx = 16*x+7.5;
                    ctry = 16*y+7.5;
                    pred_ctrx = bbox_16[y][x][4*idx]+ctrx;
                    pred_ctry = bbox_16[y][x][4*idx+1]+ctry;
                    predw = exp(bbox_16[y][x][4*idx+2])*w16;
                    predh = exp(bbox_16[y][x][4*idx+3])*h16;
                    box16[y][x][idx].x = pred_ctrx-0.5*(predw-1);
                    box16[y][x][idx].y = pred_ctry-0.5*(predh-1);
                    box16[y][x][idx].w = predw;
                    box16[y][x][idx].h = predh;
                    for (i=0;i<5;i++)
                    {
                        land16[y][x][idx][i].x=land_16[y][x][10*idx+2*i]*w16+ctrx;
                        land16[y][x][idx][i].y=land_16[y][x][10*idx+2*i+1]*h16+ctry;
                    }
                }
            }
        }
    }

    if (valid_8)
    {
        for (y=0;y<48;y++)
        {
            for (x=0;x<80;x++)
            {
                for (idx=0;idx<2;idx++)
                {
                    if (prob8[160*y+2*x+idx][0]<= 0.001)   //should not parse it if score too low
                        continue;
                    if (idx == 0)h8=w8=32;    //just for test
                    else h8=w8=16;
                    ctrx = 8*x+7.5;
                    ctry = 8*y+7.5;
                    pred_ctrx = bbox_8[y][x][4*idx]+ctrx;
                    pred_ctry = bbox_8[y][x][4*idx+1]+ctry;
                    predw = exp(bbox_8[y][x][4*idx+2])*w8;
                    predh = exp(bbox_8[y][x][4*idx+3])*h8;
                    box8[y][x][idx].x = pred_ctrx-0.5*(predw-1);
                    box8[y][x][idx].y = pred_ctry-0.5*(predh-1);
                    box8[y][x][idx].w = predw-1;
                    box8[y][x][idx].h = predh-1;
                    for (i=0;i<5;i++)
                    {
                        land8[y][x][idx][i].x=land_8[y][x][10*idx+2*i]*w8+ctrx;
                        land8[y][x][idx][i].y=land_8[y][x][10*idx+2*i+1]*h8+ctry;
                    }
                }
            }
        }
    }
    if (valid_32 == 1)
    {
        retina_do_nms_sort(pbox32, prob32, 480, 0.1);
        if (valid_16 == 1)
        {
            retina_do_nms_sort(pbox16, prob16, 1920, 0.1);
            do_global_sort(pbox32,pbox16,prob32,prob16,480,1920,0.8);
            if (valid_8 == 1)
            {
                retina_do_nms_sort(pbox8, prob8, 7680, 0.1);
                do_global_sort(pbox16, pbox8, prob16, prob8, 1920,7680,0.8);
                retina_point5_result(480, 0.8, pbox32, prob32, pland32, pface_landmark5_result);
                retina_point5_result(1920, 0.8, pbox16, prob16,pland16, pface_landmark5_result);
                retina_point5_result(7680, 0.8, pbox8, prob8,pland8, pface_landmark5_result);
            }
            else
            {
                retina_point5_result(480, 0.8, pbox32, prob32,pland32, pface_landmark5_result);
                retina_point5_result(1920, 0.8, pbox16, prob16,pland16, pface_landmark5_result);
            }
        }
        else
        {
        retina_point5_result(480, 0.6, pbox32, prob32, pland32, pface_landmark5_result);
        }
    }

    if (valid_16 == 1 && valid_32 == 0 )
    {
        retina_do_nms_sort(pbox16, prob16, 1920, 0.1);
        if (valid_8 == 1)
        {
            retina_do_nms_sort(pbox8, prob8, 7680, 0.1);
            do_global_sort(pbox16,pbox8,prob16,prob8,1920,7680,0.8);
            retina_point5_result(1920, 0.8, pbox16, prob16,pland16, pface_landmark5_result);
            retina_point5_result(7680, 0.8, pbox8, prob8,pland8, pface_landmark5_result);
        }
        else retina_point5_result(1920, 0.8, pbox16, prob16,pland16, pface_landmark5_result);
    }

    if (valid_8 == 1 && valid_16 == 0 && valid_32 == 0 )
    {
        retina_do_nms_sort(pbox8, prob8, 7680, 0.2);
        retina_point5_result(7680, 0.4, pbox8, prob8, pland8, pface_landmark5_result);
    }
    return 0;
}
void* postprocess_faceland5(nn_output *pout)
{
    float *buffer;
    unsigned int sz ;
    unsigned int i;
    static face_landmark5_out_t face_landmark5_result;
    memset(&face_landmark5_result,0,sizeof(point_t));
    for (i=0;i<pout->num;i++)
    {
        buffer = (float *)pout->out[i].buf;
        sz= pout->out[i].size;
        switch (i)
        {
            case 0:
                memcpy(prob_32,buffer,sz);
                break;
            case 1:
                memcpy(bbox_32,buffer,sz);
                break;
            case 2:
                memcpy(land_32,buffer,sz);
                break;
            case 3:
                memcpy(prob_16,buffer,sz);
                break;
            case 4:
                memcpy(bbox_16,buffer,sz);
                break;
            case 5:
                memcpy(land_16,buffer,sz);
                break;
            case 6:
                memcpy(prob_8,buffer,sz);
                break;
            case 7:
                memcpy(bbox_8,buffer,sz);
                break;
            case 8:
                memcpy(land_8,buffer,sz);
                break;
            default:
                break;
        }
    }
    face_landmark5_postprocess(&face_landmark5_result);
    return (void*)(&face_landmark5_result);
}

/****************************************************************************/
/*******************     face detect postprocess      ********************/


