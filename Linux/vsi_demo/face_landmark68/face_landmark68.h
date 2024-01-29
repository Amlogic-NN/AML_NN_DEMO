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
typedef struct __nn_face_landmark_68
{
	unsigned int detNum;
	detBox facebox[MAX_DETECT_NUM];
	point_t pos[MAX_DETECT_NUM][68];
}face_landmark68_out_t;

void* postprocess_faceland68(nn_output *pout);

#ifdef __cplusplus
} //extern "C"
#endif
#endif