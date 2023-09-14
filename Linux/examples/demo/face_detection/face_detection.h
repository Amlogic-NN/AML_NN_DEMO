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
extern float bbox_32[12][20][8]; //384/32,640/32
extern float bbox_16[24][40][8];
extern float bbox_8[48][80][8];
//prob score,input
extern float prob_32[240][2][2];
extern float prob_16[960][2][2];
extern float prob_8[3840][2][2];
//land mark
extern float land_32[12][20][20]; //384/32,640/32
extern float land_16[24][40][20];
extern float land_8[48][80][20];

extern float p_bbox_32[8][12][20]; //384/32,640/32
extern float p_bbox_16[8][24][40];
extern float p_bbox_8[8][48][80];
//prob score,input
extern float p_prob_32[480][2];
extern float p_prob_16[1920][2];
extern float p_prob_8[7680][2];

extern float bbox[5875][4];
extern float pprob[5875][2];
extern float llandmark[5875][10];

typedef struct detection_{
    box bbox;
    float *prob;
    float objectness;
    int classes;
    int sort_class;
}detection;
typedef struct __nn_face_detect
{
	unsigned int  detNum;
	detBox pBox[MAX_DETECT_NUM];
}face_detect_out_t;

float retina_box_iou(box a, box b);
void do_global_sort(box *boxe1,box *boxe2, float prob1[][1],float prob2[][1], int len1,int len2,float thresh);
void retina_do_nms_sort(box *boxes, float probs[][1], int total,  float thresh);
int retina_nms_comparator(const void *pa, const void *pb);
float retina_overlap(float x1, float w1, float x2, float w2);
float retina_box_intersection(box a, box b);
float retina_box_union(box a, box b);
void retina_result(int num, float thresh, box *boxes, float probs[][1],landmark *pland,face_detect_out_t* pface_det_result);
void *post_process_face_detection(nn_output *pOut);
void* postprocess_facedet(nn_output *pout);
int face_detect_postprocess(face_detect_out_t* pface_det_result );

#ifdef __cplusplus
} //extern "C"
#endif
#endif