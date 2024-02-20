#include <iostream>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>

#include "jpeglib.h"
#include "utils.h"
#include "nn_sdk.h"


int _jpeg_to_bmp(FILE * inputFile, unsigned char* bmpData, unsigned int bmpWidth, unsigned int bmpHeight)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    unsigned char *point = NULL;
    unsigned long width, height;
    unsigned short depth = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo,inputFile);
    jpeg_read_header(&cinfo,TRUE);

    cinfo.dct_method = JDCT_IFAST;

    if (bmpData == NULL)
    {
        return -1;
    }
    else
    {
        jpeg_start_decompress(&cinfo);

        width  = cinfo.output_width;
        height = cinfo.output_height;
        depth  = cinfo.output_components;
        if (width * height * depth != bmpWidth * bmpHeight * 3)
        {
           printf("wrong jpg file , the jpg file size should be %u %u %u\n",
               bmpWidth, bmpHeight, 3);
           return -1;
        }

        buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr)&cinfo, JPOOL_IMAGE, width*depth, 1);

        point = bmpData;

        while (cinfo.output_scanline < height)
        {
            jpeg_read_scanlines(&cinfo, buffer, 1);
            memcpy(point, *buffer, width * depth);
            point += width * depth;
        }

        jpeg_finish_decompress(&cinfo);
    }

    jpeg_destroy_decompress(&cinfo);

    return 0;
}

unsigned char *get_jpeg_rawData(const char *name,unsigned int width,unsigned int height)
{
    FILE *bmpFile;
    unsigned char *bmpData;
    unsigned int sz,w,h,c;
    int status;

    bmpFile = NULL;
    bmpData = NULL;
    w = width;
    h = height;
    c = 3;
    sz = w*h*3;

    bmpFile = fopen( name, "rb" );
    if (bmpFile == NULL)
    {
        printf("null returned\n");
        goto final;
    }

    bmpData = (unsigned char *)malloc(sz * sizeof(char));
    if (bmpData == NULL)
    {
        printf("null returned\n");
        goto final;
    }
    memset(bmpData, 0, sz * sizeof(char));

    status = _jpeg_to_bmp( bmpFile, bmpData, w, h);
    if (status == -1)
    {
        free(bmpData);
        fclose(bmpFile);
        return NULL;
    }

final:
    if (bmpFile)fclose(bmpFile);
    return bmpData;
}

void *read_file(const char *file_path, int *file_size)
{
    FILE *fp = NULL;
    int size = 0;
    void *buf = NULL;
    fp = fopen(file_path, "rb");
    if (NULL == fp)
    {
        printf("open file fail!\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    buf = malloc(sizeof(unsigned char) * size);

    fread(buf, 1, size, fp);

    fclose(fp);

    *file_size = size;
    return buf;
}

int find_max_index(float *buf, unsigned int length)
{
    int i = 0;
    int max_index = 0;
    for (i = 1; i < length; i++)
    {
        if (buf[i] > buf[max_index])
        {
            max_index = i;
        }
    }
    return max_index;
}

void save_file(void * buffer, const char *file_path, unsigned int file_size)
{
    FILE* fp  = NULL;
    size_t count;

    fp = fopen(file_path,"wb");

    count = fwrite(buffer,sizeof(unsigned char),file_size,fp);
    if (count != file_size)
        printf("fail to save input!\n");

    fclose(fp);
}

double cosine_similarity(char *file1, char *file2, int file_size)
{
    double dot_product = 0.0;
    double norm_1 = 0.0;
    double norm_2 = 0.0;
    unsigned char *rawdata1 = NULL;
    unsigned char *rawdata2 = NULL;
    int size1 = 0;
    int size2 = 0;

    rawdata1 = (unsigned char *)read_file(file1, &size1);
    rawdata2 = (unsigned char *)read_file(file2, &size1);

    for (int i = 0; i < file_size; i++)
    {
        dot_product += rawdata1[i] * rawdata2[i];
        norm_1 += rawdata1[i] * rawdata1[i];
        norm_2 += rawdata2[i] * rawdata2[i];
    }

    if (rawdata1)
    {
        free(rawdata1);
        rawdata1 = NULL;
    }
    if (rawdata2)
    {
        free(rawdata2);
        rawdata2 = NULL;
    }

    return dot_product / (sqrt(norm_1) * sqrt(norm_2));
}

int bmp_read(const char *filename, int *nRGB, int *xsize, int *ysize)
{
    char fname_bmp[1024];
    int temp, width, height;
    FILE *fp;
    int nY=0;
    int nX=0;
    int nT=0;
    int nCnt=0;
    unsigned char header[54];
    unsigned char tRGB[3];
    int lineBytes = 0;

    sprintf(fname_bmp, "%s", filename);

    if (!(fp = fopen(fname_bmp, "rb")))
	{
		printf("#ERROR: Can't read the file %s \n", fname_bmp);
		exit(1);
	}

    fread(header, sizeof(unsigned char), 54, fp);
    temp  = (int)header[2] + (((int)header[3])<<8) + (((int)header[4])<<16) + (((int)header[5])<<24);
    width = (int)header[18] + (((int)header[19])<<8) + (((int)header[20])<<16) + (((int)header[21])<<24);
    height= (int)header[22] + (((int)header[23])<<8) + (((int)header[24])<<16) + (((int)header[25])<<24);

    //fread(image, sizeof(unsigned char), (size_t)(long)width * height * 3, fp);

	lineBytes = (((width * 24) + 31) >> 5) << 2;
	temp      = width * 3;
	temp      = lineBytes - temp;
    for(nY=0;nY<height;nY++)
    {
        nCnt=(height-nY-1)*width;
        for(nX=0;nX<width;nX++)
        {
            for(nT=0;nT<3;nT++)
                fread(&tRGB[nT],1,1,fp); //a[0]:B; a[1]:G; a[2]:R

            nRGB[3*nCnt+0] = tRGB[2];
            nRGB[3*nCnt+1] = tRGB[1];
            nRGB[3*nCnt+2] = tRGB[0];
            nCnt++;

            if ((nX==width-1) && temp > 0) {
                for(nT=0;nT<temp;nT++)
                    fread(&tRGB[0],1,1,fp);
            }
        }
    }
    fclose(fp);

    /*(*filesize) = temp;*/
    (*xsize)    = width;
    (*ysize)    = height;

    return 0;
}

void data_to_fp32(float *new_buffer, unsigned char *old_buffer, int sz, float scale, int zero_point, int type)
{
    for (int i = 0; i < sz; i++)
    {
        if (NN_BUFFER_FORMAT_UINT8 == type)
        {
            new_buffer[i] = (float)(((uint8_t*)old_buffer)[i] - zero_point) * scale;
        }
        else if (NN_BUFFER_FORMAT_INT8 == type)
        {
            new_buffer[i] = (float)(((int8_t*)old_buffer)[i] - zero_point) * scale;
        }
    }
}

void Float16ToFloat32(int16_t* src , float* dst ,int length)
{
	signed int t1;
	signed int t2;
	signed int t3;
	float out;
	int i;
	for (i = 0 ;i < length ;i++)
	{
		t1 = src[i] & 0x7fff;                       // Non-sign bits
		t2 = src[i] & 0x8000;                       // Sign bit
		t3 = src[i] & 0x7c00;                       // Exponent

		t1 <<= 13;                              // Align mantissa on MSB
		t2 <<= 16;                              // Shift sign bit into position

		t1 += 0x38000000;                       // Adjust bias

		t1 = (t3 == 0 ? 0 : t1);                // Denormals-as-zero

		t1 |= t2;
		*((unsigned int*)&out) = t1;                 // Re-insert sign bit
		dst[i] = out;

	}
}

void vsi_data_to_fp32(float *new_buffer, unsigned char *old_buffer, int sz, float scale, int zero_point, int fl, int type)
{
	if (type == NN_BUFFER_FORMAT_UINT8)
	{
		for (int i = 0; i < sz; i++)
		{
			new_buffer[i] = (float)(((uint8_t*)old_buffer)[i] - zero_point) * scale;
		}
	}
	else if (type == NN_BUFFER_FORMAT_INT8)
	{
		if (fl >= 0)
		{
			fl = 1 << (fl);
			for (int i = 0; i < sz; i++)
			{
                new_buffer[i] = (float)((int8_t*)old_buffer)[i] * (1.0/(float)fl);
			}
		}
		else
		{
			fl = 1 << (-fl);
			for (int i = 0; i < sz; i++)
            {
                new_buffer[i] = (float)((int8_t*)old_buffer)[i] * ((float)fl);
            }
		}
	}
	else if (type == NN_BUFFER_FORMAT_INT16)
	{
		if (fl >= 0)
		{
			fl = 1 << (fl);
			for (int i = 0; i < sz/2; i++)
			{
                new_buffer[i] = (float)((int16_t*)old_buffer)[i] * (1.0/(float)fl);
			}
		}
		else
		{
			fl = 1 << (-fl);
			for (int i = 0; i < sz/2; i++)
            {
                new_buffer[i] = (float)((int16_t*)old_buffer)[i] * ((float)fl);
            }
		}
	}
	else if (type == NN_BUFFER_FORMAT_FP16 )
	{
		Float16ToFloat32((int16_t *)old_buffer ,new_buffer ,sz/2);
	}

	else if (type == NN_BUFFER_FORMAT_FP32)
	{
		memcpy(new_buffer, old_buffer, sz);
	}
	else
	{
		printf("Error: currently not support type, type = %d\n", type);
	}
}