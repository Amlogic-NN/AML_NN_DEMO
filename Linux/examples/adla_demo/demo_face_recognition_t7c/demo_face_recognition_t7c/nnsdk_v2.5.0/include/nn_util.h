/*
 * Copyright (c) 2022 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * aml_nnsdk
 */

#ifndef _NN_UTIL_H
#define _NN_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#define POWER_IDLE          0
#define POWER_ON            1
#define POWER_SUSPEND       2
#define POWER_OFF           3
#define POWER_RESET         4

typedef struct{
    float x, y;
} landmark;

typedef struct{
    float x, y, w, h, prob_obj;
} box;

typedef struct plate_box_{
    float x, y, w, h;
}plate_box;

typedef struct{
    int index;
    int classId;
    float **probs;
} sortable_bbox;

typedef struct{
    int index;
    int classId;
    float probs;  //**probs to probs
} sortable_bbox_plate;

//////////////////////nn sdk demo struct type//////////////////
typedef struct __box
{
    float x;
    float y;
    float w;
    float h;
    float score;
    float objectClass;
}detBox;

typedef struct __point
{
    float x;
    float y;
}point_t;

typedef struct __nn_image_out
{
    int height;
    int width;
    int channel;
    unsigned char *data;  //this buffer is returned by aml_module_output_get
}image_out_t;

///////////////////////////////////////some util api///////////////////////////////////////////////
int init_fb(void);
void *camera_thread_func(void *arg);
int sysfs_control_read(const char* name,char *out);
int sysfs_control_write(const char* pname,char *value);
int findtok(const char *buff,const char token,int length);
void activate_array(float *start, int num);
int entry_index(int lw, int lh, int lclasses, int loutputs, int batch, int location, int entry);
int mate(const char *src,const char *match, unsigned int size);


#ifdef __cplusplus
} //extern "C"
#endif
#endif