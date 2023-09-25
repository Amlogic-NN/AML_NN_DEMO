/****************************************************************************
*   amlogic nn api util header file
*
*   Neural Network appliction network definition some util header file
*
*   Date: 2019.8
***************************************************************************/
#ifndef _AMLOGIC_NN_SDK_POSTPROCESS_H
#define _AMLOGIC_NN_SDK_POSTPROCESS_H
#include "nn_sdk.h"
#include "nn_util.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct detection_{
    box bbox;
    float *prob;
    float objectness;
    int classes;
    int sort_class;
}detection;
typedef struct __nn_face_recognize
{
	float faceVector[512];
}face_recognize_out_t;

void* postprocess_facereg(nn_output *pout);

#ifdef __cplusplus
} //extern "C"
#endif
#endif