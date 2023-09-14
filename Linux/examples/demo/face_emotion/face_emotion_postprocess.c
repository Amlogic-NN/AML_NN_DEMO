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
#include "face_emotion.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"

/*******************     face emotion postprocess      ********************/
/****************************************************************************/

void* postprocess_emotion(nn_output *pout)
{
    float *buffer;
    int j = 0;
    unsigned int sz,i;
    unsigned int MaxClass[5]={0};
    float fMaxProb[5]={0.0};
    float *pfMaxProb = fMaxProb;
    unsigned int  *pMaxClass = MaxClass;
    static face_emotion_out_t *emotion_result = NULL;
	void *data = NULL;
	
    emotion_result = (face_emotion_out_t*)malloc(sizeof(face_emotion_out_t));
    memset(emotion_result, 0, sizeof(face_emotion_out_t));

    buffer = (float*)pout->out[0].buf;
    sz= pout->out[0].size;

    for (j = 0; j < 5; j++)
    {
        for (i=0; i<sz/4; i++)
        {
            if ((i == *(pMaxClass+0)) || (i == *(pMaxClass+1)) || (i == *(pMaxClass+2)) ||
                (i == *(pMaxClass+3)) || (i == *(pMaxClass+4)))
            {
                continue;
            }

            if (buffer[i] > *(pfMaxProb+j))
            {
                *(pfMaxProb+j) = buffer[i];
                *(pMaxClass+j) = i;
            }
        }
    }
    emotion_result->emotion= MaxClass[0];
    emotion_result->prob= fMaxProb[0];
    return (void*)emotion_result;
}

/****************************************************************************/
/*******************     face emotion postprocess      ********************/


