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

#ifndef _AMLOGIC_NN_SDK_YOLOV2_H
#define _AMLOGIC_NN_SDK_YOLOV2_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    float x, y, w, h, prob_obj;
} box;

void* post_process_all_module(aml_module_t type,nn_output *pOut);
void* postprocess_object_detect(nn_output *pout);

#ifdef __cplusplus
} //extern "C"
#endif
#endif