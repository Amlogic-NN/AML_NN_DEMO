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
#include "face_landmark68.h"
#include <time.h>
#include <math.h>
#include "nn_sdk.h"
#include "nn_util.h"


/*******************     face detect postprocess      ********************/
/****************************************************************************/


void* postprocess_faceland68(nn_output *pout)
{
    float *buffer;
    unsigned int i;
    int j;

    static face_landmark68_out_t face_landmark68_result;
    memset(&face_landmark68_result, 0, sizeof(face_landmark68_out_t));
    face_landmark68_result.detNum = pout->num;

    for (i=0;i<pout->num;i++)
    {
        buffer = (float *)pout->out[i].buf;
        for (j=0; j< 68; j++)
        {
            face_landmark68_result.pos[i][j].x = buffer[2 * j] * 60;
            face_landmark68_result.pos[i][j].y = buffer[2 * j + 1 ] * 60;
        }
    }

    return (void*)(&face_landmark68_result);
}

/****************************************************************************/
/*******************     face detect postprocess      ********************/


