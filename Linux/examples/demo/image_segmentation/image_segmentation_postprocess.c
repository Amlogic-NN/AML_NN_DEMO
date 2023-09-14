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
#include "image_segmentation.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"

/*******************     face detect postprocess      ********************/
/****************************************************************************/

void* postprocess_segmentation(nn_output *pout)
{
    float *buffer;
    int i,j,m,n;
    unsigned int sz;
    static segment_out_t *segment_result = NULL;

    float result_buffer[127][255][19];
    float max;
    int flag = 0;
    float sum;

    segment_result = (segment_out_t*)malloc(sizeof(segment_out_t));

    memset(segment_result, 0, sizeof(segment_out_t));
    segment_result->segOut.data = (unsigned char *)malloc(sizeof(char)*127*255*3);


    buffer = (float*)pout->out[0].buf;
    sz= pout->out[0].size;
    memcpy(result_buffer,buffer,sz);
    /********************** argmax and one-hot **************************/
    for (i = 0;i < 127;i++)
    {
        for (j = 0;j < 255;j++)
        {
            max = result_buffer[i][j][0];
            for (m = 0;m < 19;m++)
            {
                if (max < result_buffer[i][j][m])
                    max = result_buffer[i][j][m];
            }

            for (m = 0; m < 19;m++)
            {
                if (max > result_buffer[i][j][m])
                    result_buffer[i][j][m] = 0;
                else if (flag == 0)
                {
                    result_buffer[i][j][m] = 1;
                    flag = 1;
                } else
                    result_buffer[i][j][m] = 0;
            }
            flag = 0;
        }
    }
    /********************** matmul **************************/
    char label[19][3] = {{128, 64, 128}, {244, 35, 231}, {69, 69, 69}
                            ,{102, 102, 156}, {190, 153, 153}, {153, 153, 153}
                            ,{250, 170, 29}, {219, 219, 0}, {106, 142, 35},{152, 250, 152}, {69, 129, 180}, {219, 19, 60}
                            ,{255, 0, 0}, {0, 0, 142}, {0, 0, 69},{0, 60, 100}, {0, 79, 100}, {0, 0, 230},{119, 10, 32}};
    unsigned char output[127][255][3];
    for (i = 0; i < 127;i++)
    {
        for (j =0;j <255;j++ )
        {
            for (m = 0; m < 3;m++)
            {
                sum = 0;
                for (n = 0;n < 19;n++)
                {
                    sum = sum + result_buffer[i][j][n] * label[n][m];
                }
                output[i][j][m] = (unsigned char)sum;
            }
        }
    }
    memcpy(segment_result->segOut.data,output,127*255*3);
    segment_result->segOut.height = 127;
    segment_result->segOut.width = 255;
    segment_result->segOut.channel = 3;
    return (void*)segment_result;
}

/****************************************************************************/
/*******************     face detect postprocess      ********************/


