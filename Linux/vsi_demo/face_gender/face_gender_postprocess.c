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
#include "face_gender.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"

/*******************     gender detect postprocess      ********************/
/****************************************************************************/

void* postprocess_gender(nn_output *pout)
{
    float* buffer;
    static face_gender_out_t gender_result;
    if (pout->out[0].param->data_format == NN_BUFFER_FORMAT_FP32)
    {
        buffer = (float*)pout->out[0].buf;
        //printf("buffer[0]:%f,buffer[1]:%f\n",buffer[0],buffer[1]);
        gender_result.gender = buffer[0];
    }
    return (void*)&gender_result;
}


/****************************************************************************/
/*******************     gender detect postprocess      ********************/


