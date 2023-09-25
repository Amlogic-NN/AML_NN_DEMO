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

typedef struct __nn_face_rfb_detect
{
    unsigned int   detNum;
    detBox facebox[MAX_DETECT_NUM];
    point_t pos[MAX_DETECT_NUM][5];
}face_rfb_detect_out_t;

int face_rfb_detect_postprocess(face_rfb_detect_out_t* pface_rfb_det_result);
void* postprocess_rfb_facedet(nn_output *pout);

float rfb_overlap(float x1, float w1, float x2, float w2);
float rfb_box_intersection(box a, box b);
float rfb_box_union(box a, box b);
float rfb_box_iou(box a, box b);
int rfb_nms_comparator(const void *pa, const void *pb);
void rfb_do_nms_sort(box *boxes, float probs[][1], int total, int classes, float thresh);
void face_rfb_result(int num, float thresh, box *boxes, float probs[][1],landmark *pland,face_rfb_detect_out_t* pface_rfb_det_result);
void do_global_sort(box *boxe1,box *boxe2, float prob1[][1],float prob2[][1], int len1,int len2,float thresh);

#ifdef __cplusplus
} //extern "C"
#endif
#endif