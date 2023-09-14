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
#include "face_compare.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"


/*******************     face compare postprocess      ********************/
/****************************************************************************/
void* postprocess_facecompare(nn_output *pout)
{
    float *buffer;
    float sum=0, temp=0;
    int i;
    static face_compare_out_t face_compare_result;
    memset(&face_compare_result, 0, sizeof(face_compare_out_t));
    buffer = (float*)pout->out[0].buf;

    if (pout->out[0].param->data_format == NN_BUFFER_FORMAT_FP32)
    {
        for (i=0; i< 128; i++)
        {
            temp = buffer[i] - buffer[i + 128];
            sum += temp * temp;
        }
        temp = sqrt(sum);
    }

    face_compare_result.compareScore = temp;
    return (void*)(&face_compare_result);
}

/****************************************************************************/
/*******************     face compare postprocess      ********************/


