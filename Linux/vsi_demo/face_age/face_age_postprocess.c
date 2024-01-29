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
#include "face_age.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"

/*******************     face detect postprocess      ********************/
/****************************************************************************/

void* postprocess_age(nn_output *pout)
{
    float* buffer;
    unsigned int sz ;
    static face_age_out_t age_result;
    float age;
    float pred_age_s1[3];
    float pred_age_s2[3];
    float pred_age_s3[3];

    float local_s1[3];
    float local_s2[3];
    float local_s3[3];

    float delta_s1;
    float delta_s2;
    float delta_s3;

    unsigned int i,S1,S2,S3,lambda_local,lambda_d;
    float a,b,c;

    for (i=0;i<pout->num;i++)
    {
        buffer = (float *)pout->out[i].buf;
        sz= pout->out[i].size;
        switch (i)
        {
            case 0:
                memcpy(pred_age_s1,buffer,sz);
                break;
            case 1:
                memcpy(pred_age_s2,buffer,sz);
                break;
            case 2:
                memcpy(pred_age_s3,buffer,sz);
                break;
            case 3:
                memcpy(local_s1,buffer,sz);
                break;
            case 4:
                memcpy(local_s2,buffer,sz);
                break;
            case 5:
                memcpy(local_s3,buffer,sz);
                break;
            case 6:
                memcpy(&delta_s1,buffer,sz);
                break;
            case 7:
                memcpy(&delta_s2,buffer,sz);
                break;
            case 8:
                memcpy(&delta_s3,buffer,sz);
                break;
            default:
                break;
        }
    }
    S1 = 3;
    S2 = 3;
    S3 = 3;
    lambda_local = 1;
    lambda_d = 1;
    a = 0;
    b = 0;
    c = 0;

    for (i = 0;i < 3;i++)
    {
        a = a + (i + lambda_local * local_s1[i]) * pred_age_s1[i];
    }
    a = a /(S1*(1 + lambda_d * delta_s1));
    for (i = 0;i < S2;i++)
    {
        b = b + (i + lambda_local * local_s2[i]) * pred_age_s2[i];
    }
    b = b /(S1*(1 + lambda_d * delta_s1)) / (S2*(1+lambda_d*delta_s2)) ;

    for (i = 0;i < S3;i++)
    {
        c = c + (i + lambda_local * local_s3[i]) * pred_age_s3[i];
    }
    c = c /(S1*(1 + lambda_d * delta_s1)) / (S2*(1+lambda_d*delta_s2)) / (S3*(1+lambda_d*delta_s3));

    age = (a+b+c)*101;
    age_result.age = (int)age;
    return (void*)&age_result;
}

/****************************************************************************/
/*******************     face detect postprocess      ********************/


