#include <iostream>
#include <stdlib.h>
#include <float.h>

#include "postprocess.h"


void* process_top5_f32(float *buf, unsigned int num)
{
    static img_classify_out_t clsout;
    int i = 0, j = 0, k = 0;
    unsigned int MaxClass[5] = {0};
    float fMaxProb[5] = {-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX};
    // buf[0] = 17.0;

    for (i = 0; i < num; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (buf[i] > fMaxProb[j])
            {
                for (k = 4; k > j; k--)
                {
                    fMaxProb[k] = fMaxProb[k - 1];
                    MaxClass[k] = MaxClass[k - 1];
                }
                fMaxProb[j] = buf[i];
                MaxClass[j] = i;
                break;
            }
        }
    }

    for (i=0; i<5; i++)
    {
        printf("top %d:score--%f,class--%d\n", i, fMaxProb[i], MaxClass[i]);
        clsout.score[i] = (float)fMaxProb[i];
        clsout.topClass[i] = MaxClass[i];
    }
    return (void *)&clsout;
}

void* process_top5_i8(int8_t *buf, unsigned int num)
{
    static img_classify_out_t clsout;
    int i = 0, j = 0, k = 0;
    unsigned int MaxClass[5] = {0};
    int8_t fMaxProb[5] = {-128, -128, -128, -128, -128};

    for (i = 0; i < num; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (buf[i] > fMaxProb[j])
            {
                for (k = 4; k > j; k--)
                {
                    fMaxProb[k] = fMaxProb[k - 1];
                    MaxClass[k] = MaxClass[k - 1];
                }
                fMaxProb[j] = buf[i];
                MaxClass[j] = i;
                break;
            }
        }
    }

    for (i=0; i<5; i++)
    {
        printf("top %d:score--%d,class--%d\n", i, fMaxProb[i], MaxClass[i]);
        clsout.score[i] = (float)fMaxProb[i];
        clsout.topClass[i] = MaxClass[i];
    }
    return (void *)&clsout;
}

void* process_top5_u8(uint8_t *buf, unsigned int num)
{
    static img_classify_out_t clsout;
    int i = 0, j = 0, k = 0;
    unsigned int MaxClass[5] = {0};
    uint8_t fMaxProb[5] = {0, 0, 0, 0, 0};

    for (i = 0; i < num; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (buf[i] > fMaxProb[j])
            {
                for (k = 4; k > j; k--)
                {
                    fMaxProb[k] = fMaxProb[k - 1];
                    MaxClass[k] = MaxClass[k - 1];
                }
                fMaxProb[j] = buf[i];
                MaxClass[j] = i;
                break;
            }
        }
    }

    for (i=0; i<5; i++)
    {
        printf("top %d:score--%d,class--%d\n", i, fMaxProb[i], MaxClass[i]);
        clsout.score[i] = (float)fMaxProb[i];
        clsout.topClass[i] = MaxClass[i];
    }
    return (void *)&clsout;
}

void process_top5_i8_sta_top15(int8_t *buf,int num, int zeroPoint, float scale, unsigned int label, int *top1_ture, int *top5_ture)
{
    int i = 0, j = 0, k = 0;
    int MaxClass[5]={0};
    int8_t fMaxProb[5]={-128,-128,-128,-128,-128};

    for (i = 0; i < num; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (buf[i] > fMaxProb[j])
            {
                for (k = 4; k > j; k--)
                {
                    fMaxProb[k] = fMaxProb[k - 1];
                    MaxClass[k] = MaxClass[k - 1];
                }
                fMaxProb[j] = buf[i];
                MaxClass[j] = i;
                break;
            }
        }
    }

    printf("zeroPoint %d  scale %f\n",zeroPoint, scale);
    for (i = 0; i < 5; i++)
    {
        printf("top %d:score--%f,class--%d\n", i, (float)(fMaxProb[i] - zeroPoint) * scale, MaxClass[i]);
    }
    if (MaxClass[0] == label)
    {
        *top1_ture += 1;
        *top5_ture += 1;
    }
    else
    {
        for (i = 1; i < 5; i++)
        {
            if (MaxClass[i] == label)
            {
                *top5_ture += 1;
            }
        }
        
    }
    printf("Top1 matched picture: %d ; Top5 matched pictures: %d\n\n",*top1_ture,*top5_ture);
}