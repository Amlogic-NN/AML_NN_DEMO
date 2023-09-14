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
#include "face_recognize.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"

/*******************     face detect postprocess      ********************/
/****************************************************************************/

void* postprocess_facereg(nn_output *pout)
{
    float *buffer;
    int i;
    static face_recognize_out_t face_recognize_result;
    memset(&face_recognize_result, 0, sizeof(face_recognize_out_t));
    if (pout->out[0].param->data_format == NN_BUFFER_FORMAT_FP32)
    {
        buffer = (float*)pout->out[0].buf;
        for (i=0;i<512;i++)
        {
            face_recognize_result.faceVector[i] = buffer[i];
        }
    }
    return (void*)(&face_recognize_result);
}

/****************************************************************************/
/*******************     face detect postprocess      ********************/


