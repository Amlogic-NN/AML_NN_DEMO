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
typedef struct __nn_obj_detect
{
	unsigned int  detNum;
	detBox *pBox;
}obj_detect_out_t;
typedef struct __nn_head_detect
{
	obj_detect_out_t headOut;
}head_det_out_t;

void* postprocess_headdet(nn_output *pout);

float box_iou(box a, box b);
float sigmod(float x);
void do_nms_sort(box *boxes, float **probs, int total, int classes, float thresh);
int nms_comparator(const void *pa, const void *pb);
float box_union(box a, box b);
float box_intersection(box a, box b);
float overlap(float x1, float w1, float x2, float w2);

#ifdef __cplusplus
} //extern "C"
#endif
#endif