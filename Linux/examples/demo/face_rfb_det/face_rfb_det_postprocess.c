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
#include "face_rfb_det.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"


static float threshold = 0.6;
static float nms_sort = 0.4;
static float global_sort = 0.6;

float bbox[5875][4];
float pprob[5875][2];
float llandmark[5875][10];

static float ppbbox[23500] = {0};
static float ppllandmark[58750] = {0};
//static landmark *pllandmark;

static float  rbbox_8[40][40][12]; //384/32,640/32
static float  rbbox_16[20][20][8];
static float  rbbox_32[10][10][8];
static float  rbbox_64[5][5][12];

static float *rbbox8;
static float *rbbox16;
static float *rbbox32;
static float *rbbox64;

//land mark
static float  rland_8[40][40][30];
static float  rland_16[20][20][20];
static float  rland_32[10][10][20]; //384/32,640/32
static float  rland_64[5][5][30]; //384/32,640/32

/*****************************************/

/***********************************************/
static float rprob8[4800][1];
static float rprob16[800][1];
static float rprob32[200][1];
static float rprob64[75][1];
//output box
static box rbox8[40][40][3];
static box *rpbox8;
static box rbox16[20][20][2];
static box *rpbox16;
static box rbox32[10][10][2];
static box *rpbox32;
static box rbox64[5][5][3];
static box *rpbox64;
//landmark
static landmark rland8[40][40][3][5];
static landmark *rpland8;
static landmark rland16[20][20][2][5];
static landmark *rpland16;
static landmark rland32[10][10][2][5];
static landmark *rpland32;
static landmark rland64[5][5][3][5];
static landmark *rpland64;


int g_detect_number = 230;
typedef struct{
    int index;
    int classId;
    float probs;  //**probs to probs
} sortable_bbox_rfb;

/****************************************************************************/
/*******************     rfb_postprocess util********************/

float rfb_overlap(float x1, float w1, float x2, float w2)
{
    float l1 = x1 - w1/2;
    float l2 = x2 - w2/2;
    float left = l1 > l2 ? l1 : l2;
    float r1 = x1 + w1/2;
    float r2 = x2 + w2/2;
    float right = r1 < r2 ? r1 : r2;
    return right - left;
}

float rfb_box_intersection(box a, box b)
{
    float area = 0;
    float w = rfb_overlap(a.x, a.w, b.x, b.w);
    float h = rfb_overlap(a.y, a.h, b.y, b.h);
    if (w < 0 || h < 0)
        return 0;
    area = w*h;
    return area;
}

float rfb_box_union(box a, box b)
{
    float i = rfb_box_intersection(a, b);
    float u = a.w*a.h + b.w*b.h - i;
    return u;
}

float rfb_box_iou(box a, box b)
{
    return rfb_box_intersection(a, b)/rfb_box_union(a, b);
}

int rfb_nms_comparator(const void *pa, const void *pb)
{
    sortable_bbox_rfb a = *(sortable_bbox_rfb *)pa;
    sortable_bbox_rfb b = *(sortable_bbox_rfb *)pb;
    //printf("before diff,%f,%f\n",a.probs,b.probs);
    float diff = a.probs - b.probs;
    //printf("after diff\n");
    if (diff < 0) return 1;
    else if (diff > 0) return -1;
    return 0;
}

void rfb_do_nms_sort(box *boxes, float probs[][1], int total, int classes, float thresh)
{
    int i, j, k;

    sortable_bbox_rfb *s = (sortable_bbox_rfb *)calloc(total, sizeof(sortable_bbox_rfb));
    if (s == NULL)
    {
        printf("terrible calloc fail\n");
        return;
    }
    for (i = 0; i < total; ++i) {
        s[i].index = i;
        s[i].classId = 0;
        s[i].probs = probs[i][0];
    }

    for (k = 0; k < classes; ++k) {
        for (i = 0; i < total; ++i) {
            s[i].classId = k;
        }
        qsort(s, total, sizeof(sortable_bbox_rfb), rfb_nms_comparator);
        for (i = 0; i < total; ++i)
        {
            if (probs[s[i].index][k] <= 0.02)  //zxw
            {
                probs[s[i].index][k] = 0; //zxw;
                continue;
            }
            for (j = i+1; j < total; ++j) {
                box b = boxes[s[j].index];
                if (rfb_box_iou(boxes[s[i].index], b) > thresh) {
                    probs[s[j].index][k] = 0;
                }
            }
        }
    }
    //printf("do nms over\n");
    free(s);
}

void face_rfb_result(int num, float thresh, box *boxes, float probs[][1],landmark *pland,face_rfb_detect_out_t* pface_rfb_det_result)
{

    int i,j;
    int detect_num = pface_rfb_det_result->detNum;
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
                //printf("prob = %f\n",prob );
                pface_rfb_det_result->facebox[detect_num].x = boxes[i].x / 320.0;
                pface_rfb_det_result->facebox[detect_num].y = boxes[i].y / 320.0;
                pface_rfb_det_result->facebox[detect_num].w = boxes[i].w / 320.0;
                pface_rfb_det_result->facebox[detect_num].h = boxes[i].h / 320.0;
                if (pface_rfb_det_result->facebox[detect_num].x <= 0 ) pface_rfb_det_result->facebox[detect_num].x =0.000001;
                if (pface_rfb_det_result->facebox[detect_num].y <= 0 ) pface_rfb_det_result->facebox[detect_num].y =0.000001;
                if (pface_rfb_det_result->facebox[detect_num].w <= 0 ) pface_rfb_det_result->facebox[detect_num].w =0.000001;
                if (pface_rfb_det_result->facebox[detect_num].h <= 0 ) pface_rfb_det_result->facebox[detect_num].h =0.000001;
                if (pface_rfb_det_result->facebox[detect_num].x >= 1 ) pface_rfb_det_result->facebox[detect_num].x =0.999999;
                if (pface_rfb_det_result->facebox[detect_num].y >= 1 ) pface_rfb_det_result->facebox[detect_num].y =0.999999;
                if (pface_rfb_det_result->facebox[detect_num].w >= 1 ) pface_rfb_det_result->facebox[detect_num].w =0.999999;
                if (pface_rfb_det_result->facebox[detect_num].h >= 1 ) pface_rfb_det_result->facebox[detect_num].h =0.999999;
                //printf("boxes[%d].x = %f,boxes[%d].y = %f,boxes[%d].w = %f,boxes[%d].h = %f\n",i,boxes[i].x,i,boxes[i].y,i,boxes[i].w,i,boxes[i].h );
                for (j=0 ;j <5 ; j++)
                {
                    pface_rfb_det_result->pos[detect_num][j].x = pland[i * 5 + j].x / 320.0;
                    pface_rfb_det_result->pos[detect_num][j].y = pland[i * 5 + j].y / 320.0;
                    if (pface_rfb_det_result->pos[detect_num][j].x <= 0) pface_rfb_det_result->pos[detect_num][j].x=0.001;
                    if (pface_rfb_det_result->pos[detect_num][j].x >= 1) pface_rfb_det_result->pos[detect_num][j].x=0.997;
                    if (pface_rfb_det_result->pos[detect_num][j].y <= 0) pface_rfb_det_result->pos[detect_num][j].y=0.001;
                    if (pface_rfb_det_result->pos[detect_num][j].y >= 1) pface_rfb_det_result->pos[detect_num][j].y=0.997;
                    //printf("point number =%d,rawData-X:%.5f,Y:%.5f\n" , j, pface_rfb_det_result->pos[detect_num][j].x, pface_rfb_det_result->pos[detect_num][j].y);
                }
                detect_num++;
            }
        }
    }
    pface_rfb_det_result->detNum = detect_num;
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
                    if (rfb_box_iou(boxe1[i], boxe2[j]) > 0.1)
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

/****************************************************************************/
/*******************     rfb_postprocess util   ********************/

/****************************************************************************/
/*******************     face detect postprocess      ********************/

int face_rfb_detect_postprocess(face_rfb_detect_out_t* pface_rfb_det_result )
{
    int i = 0,j = 0,k = 0,x,y;
    int idx;
    int h64,w64,h32,w32,h16,w16,h8,w8;
    float pred_ctrx,pred_ctry,predw,predh;
    int valid_8 = 0,valid_16 = 0,valid_32 = 0,valid_64 = 0;
    int num = 0,p = 0;

    rbbox8  = (float *)rbbox_8;
    rbbox16 = (float *)rbbox_16;
    rbbox32 = (float *)rbbox_32;
    rbbox64 = (float *)rbbox_64;

    rpbox8  = (box *)rbox8;
    rpbox16 = (box *)rbox16;
    rpbox32 = (box *)rbox32;
    rpbox64 = (box *)rbox64;

    rpland8  = (landmark *)rland8;
    rpland16 = (landmark *)rland16;
    rpland32 = (landmark *)rland32;
    rpland64 = (landmark *)rland64;


    for (i=0;i<5875;i++)
    {
        for (j=0;j<4;j++)
        {
            ppbbox[k] = bbox[i][j];
            k++;
        }
    }
    k = 0;

    for (i=0;i<5875;i++)
    {
        for (j=0;j<10;j++)
        {
            ppllandmark[k] = llandmark[i][j];
            k++;
        }
    }
    k = 0;

    for (i=0;i<40;i++)
    {
        for (j=0;j<40;j++)
        {
            for (idx=0;idx<12;idx++)
            {
                rbbox_8[i][j][idx] = ppbbox[k];
                k++;
            }
            for (idx=0;idx<30;idx++)
            {
                rland_8[i][j][idx] = ppllandmark[p];
                p++;
            }

        }
    }

    for (i=0;i<20;i++)
    {
        for (j=0;j<20;j++)
        {
            for (idx=0;idx<8;idx++)
            {
                rbbox_16[i][j][idx] = ppbbox[k];
                k++;
            }
            for (idx=0;idx<20;idx++)
            {
                rland_16[i][j][idx] = ppllandmark[p];
                p++;
            }
        }
    }


    for (i=0;i<10;i++)
    {
        for (j=0;j<10;j++)
        {
            for (idx=0;idx<8;idx++)
            {
                rbbox_32[i][j][idx] = ppbbox[k];
                k++;
            }
            for (idx=0;idx<20;idx++)
            {
                rland_32[i][j][idx] = ppllandmark[p];
                p++;
            }
        }
    }


    for (i=0;i<5;i++)
    {
        for (j=0;j<5;j++)
        {
            for (idx=0;idx<12;idx++)
            {
                rbbox_64[i][j][idx] = ppbbox[k];
                k++;
            }
            for (idx=0;idx<30;idx++)
            {
                rland_64[i][j][idx] = ppllandmark[p];
                p++;
            }
        }
    }

    k = 0;
    for (i=0;i<4800;i++)
    {
        rprob8[k][0] = pprob[i][1];
        if (rprob8[k][0] < threshold)
        {
            rprob8[k][0] = 0;
        }
        else
        {
            valid_8 = 1;
            num++;
        }
        k++;
    }
    k = 0;
    num =0;

    for (i=0;i<800;i++)
    {
        rprob16[k][0] = pprob[i+4800][1];
        if (rprob16[k][0] < threshold)
        {
            rprob16[k][0] = 0;
        }
        else
        {
            valid_16 = 1;
            num++;
        }
        k++;
    }
    k = 0;
    num =0;

    for (i=0;i<200;i++)
    {
        rprob32[k][0] = pprob[i+5600][1];
        if (rprob32[k][0] < threshold)
        {
            rprob32[k][0] = 0;
        }
        else
        {
            valid_32 = 1;
            num++;
        }
        k++;
    }

    k = 0;
    num =0;

    for (i=0;i<75;i++)
    {
        rprob64[k][0] = pprob[i+5800][1];
        if (rprob64[k][0] < threshold)
        {
            rprob64[k][0] = 0;
        }
        else
        {
            valid_64 = 1;
            num++;
        }
        k++;
    }

    k = 0;
    num =0;

    if (valid_8)
    {
        for (y=0;y<40;y++)
        {
            for (x=0;x<40;x++)
            {
                for (idx=0;idx<3;idx++)
                {
                    if (idx == 0)
                        h8=w8=10;
                    else if (idx == 1)
                        h8=w8=16;
                    else
                        h8=w8=24;

                    float s_kx = w8;
                    float s_ky = h8 ;
                    float cx = (x + 0.5) * 8;
                    float cy = (y + 0.5) * 8;

                    pred_ctrx = cx + rbbox_8[y][x][4*idx] * 0.1 * s_kx;
                    pred_ctry = cy + rbbox_8[y][x][4*idx+1] * 0.1 * s_ky;
                    predw = exp((rbbox_8[y][x][4*idx+2])*0.2) * s_kx;
                    predh = exp((rbbox_8[y][x][4*idx+3])*0.2) * s_ky;

                    rbox8[y][x][idx].x = (pred_ctrx-0.5*(predw));
                    rbox8[y][x][idx].y = (pred_ctry-0.5*(predh));
                    rbox8[y][x][idx].w = predw;
                    rbox8[y][x][idx].h = predh;
                    //printf("8 x=%f,y=%f,w=%f,h=%f\n",rbox8[y][x][idx].x,rbox8[y][x][idx].y,rbox8[y][x][idx].w,rbox8[y][x][idx].h);

                    for (i=0;i<5;i++)
                    {
                        rland8[y][x][idx][i].x=(cx + rland_8[y][x][10*idx+2*i]* 0.1 * s_kx);
                        rland8[y][x][idx][i].y=(cy + rland_8[y][x][10*idx+2*i+1]* 0.1 * s_ky);
                        //printf("rland8[%d][%d][%d][%d].x = %f,  rland8[%d][%d][%d][%d].y = %f\n",y,x,idx,i,rland8[y][x][idx][i].x ,y,x,idx,i,rland8[y][x][idx][i].y );
                    }
                }
            }
        }
    }

    if (valid_16)
    {
        for (y=0;y<20;y++)
        {
            for (x=0;x<20;x++)
            {
                for (idx=0;idx<2;idx++)
                {
                    if (idx == 0)
                        h16=w16=32;
                    else if (idx == 1)
                        h16=w16=48;

                    float s_kx = w16;
                    float s_ky = h16;
                    float cx = (x + 0.5) * 16;
                    float cy = (y + 0.5) * 16;

                    pred_ctrx = cx + rbbox_16[y][x][4*idx] * 0.1 * s_kx;
                    pred_ctry = cy + rbbox_16[y][x][4*idx+1] * 0.1 * s_ky;
                    predw = exp((rbbox_16[y][x][4*idx+2])*0.2) * s_kx;
                    predh = exp((rbbox_16[y][x][4*idx+3])*0.2) * s_ky;

                    rbox16[y][x][idx].x = (pred_ctrx-0.5*(predw));
                    rbox16[y][x][idx].y = (pred_ctry-0.5*(predh));
                    rbox16[y][x][idx].w = predw;
                    rbox16[y][x][idx].h = predh;
                    //printf("16 x=%f,y=%f,w=%f,h=%f\n",rbox16[y][x][idx].x,rbox16[y][x][idx].y,rbox16[y][x][idx].w,rbox16[y][x][idx].h);

                    for (i=0;i<5;i++)
                    {
                        rland16[y][x][idx][i].x=(cx + rland_16[y][x][10*idx+2*i]* 0.1 * s_kx);
                        rland16[y][x][idx][i].y=(cy + rland_16[y][x][10*idx+2*i+1]* 0.1 * s_ky);
                    }
                }
            }
        }
    }

    if (valid_32)
    {
        for (y=0;y<10;y++)
        {
            for (x=0;x<10;x++)
            {
                for (idx=0;idx<2;idx++)
                {
                    if (idx == 0)
                        h32=w32=64;
                    else if (idx == 1)
                        h32=w32=96;

                    float s_kx = w32;
                    float s_ky = h32;
                    float cx = (x + 0.5) * 32;
                    float cy = (y + 0.5) * 32;

                    pred_ctrx = cx + rbbox_32[y][x][4*idx] * 0.1 * s_kx;
                    pred_ctry = cy + rbbox_32[y][x][4*idx+1] * 0.1 * s_ky;
                    predw = exp((rbbox_32[y][x][4*idx+2])*0.2) * s_kx;
                    predh = exp((rbbox_32[y][x][4*idx+3])*0.2) * s_ky;

                    rbox32[y][x][idx].x = (pred_ctrx-0.5*(predw));
                    rbox32[y][x][idx].y = (pred_ctry-0.5*(predh));
                    rbox32[y][x][idx].w = predw;
                    rbox32[y][x][idx].h = predh;
                    //printf("32 x=%f,y=%f,w=%f,h=%f\n",rbox32[y][x][idx].x,rbox32[y][x][idx].y,rbox32[y][x][idx].w,rbox32[y][x][idx].h);

                    for (i=0;i<5;i++)
                    {
                        rland32[y][x][idx][i].x=(cx + rland_32[y][x][10*idx+2*i]* 0.1 * s_kx);
                        rland32[y][x][idx][i].y=(cy + rland_32[y][x][10*idx+2*i+1]* 0.1 * s_ky);
                    }
                }
            }
        }
    }

    if (valid_64)
    {
        for (y=0;y<5;y++)
        {
            for (x=0;x<5;x++)
            {
                for (idx=0;idx<3;idx++)
                {
                    //****************************************//
                    if (idx == 0)
                        h64=w64=128;
                    else if (idx == 1)
                        h64=w64=192;
                    else
                        h64=w64=256;

                    float s_kx = w64;
                    float s_ky = h64;
                    float cx = (x + 0.5) * 64;
                    float cy = (y + 0.5) * 64;

                    pred_ctrx = cx + rbbox_64[y][x][4*idx] * 0.1 * s_kx;
                    pred_ctry = cy + rbbox_64[y][x][4*idx+1] * 0.1 * s_ky;
                    predw = exp((rbbox_64[y][x][4*idx+2])*0.2) * s_kx;
                    predh = exp((rbbox_64[y][x][4*idx+3])*0.2) * s_ky;

                    rbox64[y][x][idx].x = (pred_ctrx-0.5*(predw));
                    rbox64[y][x][idx].y = (pred_ctry-0.5*(predh));
                    rbox64[y][x][idx].w = predw;
                    rbox64[y][x][idx].h = predh;
                    //printf("64 x=%f,y=%f,w=%f,h=%f\n",rbox64[y][x][idx].x,rbox64[y][x][idx].y,rbox64[y][x][idx].w,rbox64[y][x][idx].h);

                    for (i=0;i<5;i++)
                    {
                        rland64[y][x][idx][i].x=(cx + rland_64[y][x][10*idx+2*i]* 0.1 * s_kx);
                        rland64[y][x][idx][i].y=(cy + rland_64[y][x][10*idx+2*i+1]* 0.1 * s_ky);
                    }

                }
            }
        }
    }

    if (valid_64 == 1)
    {
        rfb_do_nms_sort(rpbox64, rprob64, 75, 1, nms_sort);
        if (valid_32 == 1)
        {
            rfb_do_nms_sort(rpbox32, rprob32, 200, 1, nms_sort);
            do_global_sort(rpbox64,rpbox32,rprob64,rprob32,75,200,global_sort);
            if (valid_16 == 1)
            {
                rfb_do_nms_sort(rpbox16, rprob16, 800, 1, nms_sort);
                do_global_sort(rpbox32, rpbox16, rprob32, rprob16, 200,800,global_sort);
                if (valid_8 == 1)
                {
                    rfb_do_nms_sort(rpbox8, rprob8, 4800, 1, nms_sort);
                    do_global_sort(rpbox16, rpbox8, rprob16, rprob8, 800,4800,global_sort);
                    face_rfb_result(75, threshold, rpbox64, rprob64,rpland64,pface_rfb_det_result);
                    face_rfb_result(200, threshold, rpbox32, rprob32,rpland32,pface_rfb_det_result);
                    face_rfb_result(800, threshold, rpbox16, rprob16,rpland16,pface_rfb_det_result);
                    face_rfb_result(4800, threshold, rpbox8, rprob8,rpland8,pface_rfb_det_result);
                }
                else
                {
                    face_rfb_result(75, threshold, rpbox64, rprob64,rpland64,pface_rfb_det_result);
                    face_rfb_result(200, threshold, rpbox32, rprob32,rpland32,pface_rfb_det_result);
                    face_rfb_result(800, threshold, rpbox16, rprob16,rpland16,pface_rfb_det_result);
                }
            }
            else
            {
                face_rfb_result(75, threshold, rpbox64, rprob64,rpland64,pface_rfb_det_result);
                face_rfb_result(200, threshold, rpbox32, rprob32,rpland32,pface_rfb_det_result);
            }
        }
        else face_rfb_result(75, 0.2, rpbox64, rprob64,rpland64,pface_rfb_det_result);
    }
    else
    {
        if (valid_32 == 1)
        {
            rfb_do_nms_sort(rpbox32, rprob32, 200, 1, nms_sort);
            do_global_sort(rpbox64,rpbox32,rprob64,rprob32,75,200,global_sort);
            if (valid_16 == 1)
            {
                rfb_do_nms_sort(rpbox16, rprob16, 800, 1, nms_sort);
                do_global_sort(rpbox32, rpbox16, rprob32, rprob16, 200,800,global_sort);
                if (valid_8 == 1)
                {
                    rfb_do_nms_sort(rpbox8, rprob8, 4800, 1, nms_sort);
                    do_global_sort(rpbox16, rpbox8, rprob16, rprob8, 800,4800,global_sort);
                    face_rfb_result(75, threshold, rpbox64, rprob64,rpland64,pface_rfb_det_result);
                    face_rfb_result(200, threshold, rpbox32, rprob32,rpland32,pface_rfb_det_result);
                    face_rfb_result(800, threshold, rpbox16, rprob16,rpland16,pface_rfb_det_result);
                    face_rfb_result(4800, threshold, rpbox8, rprob8,rpland8,pface_rfb_det_result);
                }
                else
                {
                    face_rfb_result(75, threshold, rpbox64, rprob64,rpland64,pface_rfb_det_result);
                    face_rfb_result(200, threshold, rpbox32, rprob32,rpland32,pface_rfb_det_result);
                    face_rfb_result(800, threshold, rpbox16, rprob16,rpland16,pface_rfb_det_result);
                }
            }
            else
            {
                face_rfb_result(200, threshold, rpbox32, rprob32,rpland32,pface_rfb_det_result);
            }
        }
        else
        {
            if (valid_16 == 1)
            {
                rfb_do_nms_sort(rpbox16, rprob16, 800, 1, nms_sort);
                if (valid_8 == 1)
                {
                    rfb_do_nms_sort(rpbox8, rprob8, 4800, 1, nms_sort);
                    do_global_sort(rpbox16, rpbox8, rprob16, rprob8, 800,4800,global_sort);
                    face_rfb_result(800, threshold, rpbox16, rprob16,rpland16,pface_rfb_det_result);
                    face_rfb_result(4800, threshold, rpbox8, rprob8,rpland8,pface_rfb_det_result);
                }
                else
                {
                    face_rfb_result(800, threshold, rpbox16, rprob16,rpland16,pface_rfb_det_result);
                }
            }
            else
            {
                if (valid_8 == 1)
                {
                    rfb_do_nms_sort(rpbox8, rprob8, 4800, 1, nms_sort);
                    face_rfb_result(4800, threshold, rpbox8, rprob8,rpland8,pface_rfb_det_result);
                }
            }
        }
    }

    return 0;
}

void* postprocess_rfb_facedet(nn_output *pout)
{
    float *buffer;
    unsigned int sz ;
    unsigned int i;
    static face_rfb_detect_out_t face_rfb_det_result;
    memset(&face_rfb_det_result,0,sizeof(face_rfb_detect_out_t));
    for (i=0;i<pout->num;i++)
    {
        buffer = (float *)pout->out[i].buf;
        sz= pout->out[i].size;
        switch (i)
        {
            case 0:
                memcpy(bbox,buffer,sz);
                break;
            case 1:
                memcpy(pprob,buffer,sz);
                break;
            case 2:
                memcpy(llandmark,buffer,sz);
                break;
            default:
                break;
        }
    }
    face_rfb_detect_postprocess(&face_rfb_det_result);
    return (void*)(&face_rfb_det_result);
}


/****************************************************************************/
/*******************     face detect postprocess      ********************/
