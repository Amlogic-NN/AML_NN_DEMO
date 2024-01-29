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

/***************************  image classify top5   **************************************/
void process_top5(float *buf,unsigned int num,img_classify_out_t* clsout)
{
    int j = 0;
    unsigned int MaxClass[5]={0};
    float fMaxProb[5]={0.0};

    float *pfMaxProb = fMaxProb;
    unsigned int  *pMaxClass = MaxClass,i = 0;

    for (j = 0; j < 5; j++)
    {
        for (i=0; i<num; i++)
        {
            if ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
            {
                continue;
            }

            if (buf[i] > *(pfMaxProb+j))
            {
                *(pfMaxProb+j) = buf[i];
                *(pMaxClass+j) = i;
            }
        }
    }
    for (i=0; i<5; i++)
    {
        if (clsout == NULL)
        {
            printf("%3d: %8.6f\n", MaxClass[i], fMaxProb[i]);
        }
        else
        {
            clsout->score[i] = fMaxProb[i];
            clsout->topClass[i] = MaxClass[i];
        }
    }
}
/***************************  image classify top5   **************************************/

/***************************  IMAGE_CLASSIFY postprocess **************************************/
void* postprocess_classify(nn_output *pout)
{
    static img_classify_out_t cls_result;
    memset(&cls_result,0,sizeof(img_classify_out_t));
    if (pout->out[0].param->data_format == NN_BUFFER_FORMAT_FP32)
    {
        process_top5((float*)pout->out[0].buf,pout->out[0].size/sizeof(float),&cls_result);
    }

    return (void*)&cls_result;
}
/***************************  IMAGE_CLASSIFY postprocess **************************************/

void *post_process_all_module(aml_module_t type,nn_output *pOut)
{
	aml_module_t nettype = type;
	void *data = NULL;

	switch (nettype)
	{
	case IMAGE_CLASSIFY:
		data  = postprocess_classify(pOut);
		break;
	default:
		break;
	}
	return data;
}
