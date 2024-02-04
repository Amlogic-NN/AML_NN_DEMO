/*
 * Copyright (c) 2022 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * aml_nnsdk
 */

#ifndef _NN_SDK_H
#define _NN_SDK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////some value define////////////////////////
#define MAX_NAME_LENGTH             64
#define INPUT_MAX_NUM               64
#define OUTPUT_MAX_NUM              64
#define INPUT_CHANNEL               3
#define MAX_DETECT_NUM              230
#define SUPPORT_NET_NUM             60
#define ADDRESS_MAX_NUM             64
#define MAX_TENSOR_NUM_DIMS         6
#define INPUT_META_NUM              1

/*=====================================================
the common type for sdk api
======================================================*/
typedef enum {
    AML_IN_PICTURE            = 0,
    AML_IN_VIDEO              = 1,
    AML_IN_CAMERA             = 2
} amlnn_input_mode_t;

typedef enum _amlnn_model_ {
    CAFFE                     = 0,
    TENSORFLOW,
    TENSORFLOWLITE,
    DARKNET,
    ONNX,
    KERAS,
    PYTORCH,
    ADLA_LOADABLE,
    MEDEL_MAX
} amlnn_model_type;

/*=================================================================================
load nbg type
==================================================================================*/
typedef enum _amlnn_nbg_type_ {
    NN_NBG_FILE               = 0,
    NN_NBG_MEMORY             = 1,
    NN_RUNTIME_FILE           = 2,
    NN_RUNTIME_MEMORY         = 3,
    NN_ADLA_FILE              = 4,
    NN_ADLA_MEMORY            = 5
} amlnn_nbg_type;

typedef enum _amlnn_input_ {
    RGB24_RAW_DATA = 0,
    TENSOR_RAW_DATA,
    QTENSOR_RAW_DATA,
    BINARY_RAW_DATA,
    INPUT_DMA_DATA,
    RAW_DATA_MAX
} amlnn_input_type;

typedef enum _nn_buffer_format_e
{
    /*! \brief A float type of buffer data */
    NN_BUFFER_FORMAT_FP32       = 0,
    /*! \brief A half float type of buffer data */
    NN_BUFFER_FORMAT_FP16       = 1,
    /*! \brief A 8 bit unsigned integer type of buffer data */
    NN_BUFFER_FORMAT_UINT8      = 2,
    /*! \brief A 8 bit signed integer type of buffer data */
    NN_BUFFER_FORMAT_INT8       = 3,
    /*! \brief A 16 bit unsigned integer type of buffer data */
    NN_BUFFER_FORMAT_UINT16     = 4,
    /*! \brief A 16 signed integer type of buffer data */
    NN_BUFFER_FORMAT_INT16      = 5,
    /*! \brief A 32 signed integer type of buffer data */
    NN_BUFFER_FORMAT_INT32      = 8
} nn_buffer_format_e;

typedef enum _nn_buffer_quantize_format_e
{
    /*! \brief Not quantized format */
    NN_BUFFER_QUANTIZE_NONE                    = 0,
    /*! \brief The data is quantized with dynamic fixed point */
    NN_BUFFER_QUANTIZE_DYNAMIC_FIXED_POINT     = 1,
    /*! \brief The data is quantized with TF asymmetric format */
    NN_BUFFER_QUANTIZE_TF_ASYMM                = 2
} nn_buffer_quantize_format_e;

typedef enum {
    IMAGE_CLASSIFY      = 0,
    OBJECT_DETECT       = 1,    /// object_detection
    FACE_DETECTION      = 2,    ///
    FACE_LANDMARK_5     = 3,    /// landmark 5 points
    FACE_LANDMARK_68    = 4,    ///< Face Landmark (68 KeyPoints)
    FACE_RECOGNIZE      = 5,    ///< Face Recognition
    FACE_COMPARISON     = 6,    ///< Face compare,whether same person
    FACE_AGE            = 7,    ///< Face age
    FACE_GENDER         = 8,    ///< Face gender
    FACE_EMOTION        = 9,    ///< Face emotion
    BODY_POSE           = 10,   ///< Body Pose
    FINGER_POSE         = 11,   ///< Finger Landmark(21 KeyPoint)
    HEAD_DETECTION      = 12,   ///< Head Detection
    CARPLATE_DETECTION  = 13,   ///< Car Plate Detection
    CARPLATE_RECOG      = 14,   ///< Car Plate Recognition
    TEXT_DETECTION      = 15,   ///< text region detect
    IMAGE_SR            = 16,   ///< image SR
    IMAGE_SEGMENTATION  = 17,   ///< image segment, based on icnet
    PERSON_DETECT       = 18,   ///< person detect
    YOLOFACE_V2         = 19,
    YOLO_V2             = 20,
    YOLO_V3             = 21,
    FACE_NET            = 22,
    FACE_RECOG_U        = 23,
    FACE_RFB_DETECTION  = 24,
    AML_PERSON_DETECT   = 25,
    AML_BODY_POSE       = 26,
    mobilenetv2         = 30,
    centerface          = 31,
    ALPD                = 32,
    ALPR                = 33,
    face_blur_v2        = 34,
    face_recog_small    = 35,
    face_reid_v2        = 36,
    landmark2d_106      = 37,
    new_car             = 38,
    old_person          = 39,
    CUSTOM_NETWORK      = 99,   ///< custom network, for user development
    MODEL_MAX           = 100    ///< max model number
} aml_module_t;

typedef enum {
    AML_OUTDATA_FLOAT32      = 0,
    AML_OUTDATA_RAW          = 1,
    AML_OUTDATA_DMA          = 2
} aml_output_format_t;

typedef enum {
    AML_NO_PERF            = 0,
    AML_PERF_INFERENCE     = 1,
    AML_PERF_OUTPUT_GET    = 2,
    AML_PERF_OUTPUT_SET    = 3
} aml_perf_mode_t;

typedef enum {
    AML_PROFILE_NONE         = 0,
    AML_PROFILE_PERFORMANCE  = 1,
    AML_PROFILE_BANDWIDTH    = 2,
    AML_PROFILE_MEMORY       = 3,
    AML_PERLAYER_RUNTIME     = 4,
    AML_PERLAYER_BANDWIDTH   = 5,
    AML_PERLAYER_OUTPUT      = 6,
    AML_PERLAYER_INPUT       = 7
} aml_profile_type_t;

typedef enum {
    AML_PERFORMANCE_MODE         = 1,
    AML_POWER_SAVE_MODE          = 2,
    AML_MINIMUM_POWER_MODE       = 3
} aml_policy_type_t;

typedef enum {
    AML_IO_VIRTUAL      = 0,
    AML_IO_PHYS         = 1,
} aml_io_format_t;

typedef enum {
    AML_OUTPUT_ORDER_DEFAULT      = 0,    //output format:  nhwc for adla, nchw for galcore
    AML_OUTPUT_ORDER_NHWC         = 1,    //output format: nhwc
    AML_OUTPUT_ORDER_NCHW         = 2,    //output format: nchw
} aml_output_order_t;

typedef  struct __aml_invoke_info_t
{
    int typeSize;
    int invoke_type; // 1: invoke_no_wait, 2: waitwithid
    int32_t timeout; //ms
    int64_t invoke_id;
} aml_invoke_info_t;

typedef  struct __amlnn_module_out_data_t
{
    int typeSize;
    aml_module_t mdType;
    aml_perf_mode_t perfMode;
    aml_output_format_t format;
    aml_output_order_t order;
    aml_invoke_info_t invoke;
} aml_output_config_t;

typedef struct _nn_buffer_create_params_t
{
    unsigned int     num_of_dims; /*!< \brief The number of dimensions specified in *sizes*/
    unsigned int      sizes[4];       /*!< \brief The pointer to an array of dimension */
    nn_buffer_format_e          data_format; /*!< \brief Data format for the tensor, see <tt>\ref nn_buffer_format_e </tt> */
    nn_buffer_quantize_format_e quant_format; /*!< \brief Quantized format see <tt>\ref nn_buffer_quantize_format_e </tt>. */
    union {
        struct {
            unsigned char fixed_point_pos; /*!< \brief Specifies the fixed point position when the input element type is int16, if 0 calculations are performed in integer math */
        } dfp;

        struct {
            float        scale;       /*!< \brief Scale value for the quantized value */
            unsigned int zeroPoint;  /*!< \brief  A 32 bit integer, in range [0, 255] */
        } affine;
     }
     quant_data; /*<! \brief The union of quantization information */
} nn_buffer_params_t;

typedef enum {
    AML_INPUT_DEFAULT      = 0,    //channel format: caffe 2 1 0 ,others 0 1 2
    AML_INPUT_MODEL_NHWC   = 1,    //channel format: 0 1 2
    AML_INPUT_MODEL_NCHW   = 2,    //channel format: 2 1 0
} aml_input_format_t;

typedef struct out_buf
{
    unsigned int size;
    char  name[MAX_NAME_LENGTH];     //output tensor name
    unsigned char *buf;
    nn_buffer_params_t *param;
    aml_output_format_t out_format;
} outBuf_t;

typedef struct __nnout
{
    int typeSize;
    unsigned int num;   /*===========output tensor number============*/
    outBuf_t out[OUTPUT_MAX_NUM];
} nn_output;

typedef enum {
    AML_INPUT_U8,
    AML_INPUT_I8,
    AML_INPUT_FP32,
} aml_input_data_type_t;

typedef struct
{
    int valid;
    int int16_type;
    int preprocess_debug;
    float mean[INPUT_CHANNEL];
    float scale;
    aml_input_format_t input_format;
    aml_input_data_type_t input_data_type;
}input_info;

typedef struct __nn_input
{
    int  typeSize;
    int  input_index;
    int  size;
    unsigned char* input;
    amlnn_input_type input_type;
    input_info info;
}nn_input;

typedef struct __assign_address
{
    unsigned int inAddr_size;
    unsigned int outAddr_size;
    aml_io_format_t io_type;
    unsigned char* inAddr[ADDRESS_MAX_NUM];
    unsigned char* outAddr[ADDRESS_MAX_NUM];
}assign_user_address_t;

typedef enum __enCoreId
{
    AML_ID_0       = 0,
    AML_ID_1       = 1,
    AML_ID_2       = 2,
    AML_ID_3       = 3,
    AML_ID_4       = 4,
    AML_ID_BUTT
} aml_encore_id;

typedef enum __aml_runtime_mode_t
{
    AML_RUNTIME_NNRT       = 0,
    AML_RUNTIME_OVX        = 1,
    AML_RUNTIME_TIM_VX     = 2,
    RUN_TIME_MODE_BUTT
} aml_runtime_mode_t;

typedef enum {
    AML_WITH_CACHE            = 0,
    AML_WITHOUT_CACHE         = 1,
} aml_cache_type_t;

typedef struct __aml_forward_ctrl_t
{
    aml_encore_id              enCoreId;       /* device target which running the seg*/
    int64_t                    invoke_id;
    int32_t                    timeout_ms;
} aml_forward_ctrl_t;

typedef enum __aml_model_type_t
{
    AML_MODEL_TYPE_ADLA_LOADABLE = 0,
    AML_MODEL_TYPE_TENSORFLOW,
    AML_MODEL_TYPE_TENSORFLOW_LITE
} aml_model_type_t;

typedef enum __aml_model_in_out_type_t
{
    AML_MODEL_IN_OUT_TYPE_MEMORY = 0,
    AML_MODEL_IN_OUT_TYPE_FILE
} aml_model_in_out_type_t;

typedef enum __aml_compiler_optimization_mode_t
{
    AML_COMPILER_OPTIMIZATION_MODE_FAST = 0,
    AML_COMPILER_OPTIMIZATION_MODE_PRECISE
} aml_compiler_optimization_mode_t;

typedef struct __aml_compiler_input_t
{
    aml_model_type_t model_type;
    aml_model_in_out_type_t input_type;

    const void* model_data;
    size_t model_size;

    const char* model_path;
} aml_compiler_input_t;

typedef struct __aml_compiler_config_t
{
    const char* hw_version;
    int32_t axi_sram_size;
    int32_t batch_multiplier;
    aml_compiler_optimization_mode_t optimization_mode;
} aml_compiler_config_t;

typedef struct __aml_compiler_allocator_t
{
    uint8_t* (*allocate)(size_t size);
    void (*deallocate)(uint8_t *p, size_t size);
} aml_compiler_allocator_t;

typedef struct __aml_compiler_metadata_t
{
    int32_t count;
    const char** names;
    const char** data;
} aml_compiler_metadata_t;

typedef struct __aml_compiler_debug_options_t
{
    bool dump_model_info;
    bool disable_fusion;
    bool disable_compression;
    bool disable_memory_optimization;
} aml_compiler_debug_options_t;

typedef struct __aml_compiler_args_t
{
    // int32_t batch_multiplier;
    int32_t compiler_only;
    int32_t set_compiler_args_flag;
    aml_compiler_input_t input;
    aml_compiler_config_t config;
    const aml_compiler_allocator_t* allocator;
    const aml_compiler_metadata_t* metadata;
    const aml_compiler_debug_options_t* debug_options;
    const char* custom_option_path;
} aml_compiler_args_t;

typedef enum __aml_hw_flag_t
{
    AML_HW_NPU     = 1,
    AML_HW_GPU     = 2,
    AML_HW_CPU     = 3
} aml_hw_flag_t;

typedef struct __aml_nn_config
{
    int typeSize;
    int length;
    const char *path;
    const char *pdata;
    amlnn_model_type modelType;
    amlnn_nbg_type nbgType;
    assign_user_address_t inOut;
    aml_forward_ctrl_t forward_ctrl;
    aml_compiler_args_t compiler_args;
    aml_hw_flag_t hw_flag;
} aml_config;

typedef struct {
    unsigned int dim_count;       /*dim count*/
    unsigned int sizes_of_dim[MAX_TENSOR_NUM_DIMS]; /*dim value,just support 4-d dim*/
    unsigned int data_format;     /*see as nn_buffer_format_e*/
    unsigned int data_type;       /*not use*/
    unsigned int quantization_format; /*see as nn_buffer_quantize_format_e*/
    int fixed_point_pos;          /*for int8/int16 QUANTIZE_DYNAMIC_FIXED_POINT*/
    float TF_scale;               /*as tf define,scale*/
    int TF_zeropoint;             /*as tf define,zeropoint*/
    char name[MAX_NAME_LENGTH];    /*not use,will used in future*/
} info_t;

typedef struct {
    unsigned int valid;
    unsigned int num;
    info_t *info;
} tensor_info;

typedef enum {
    AML_INPUT_TENSOR      = 0,
    AML_OUTPUT_TENSOR     = 1,
} aml_flush_type_t;

typedef enum {
    AML_HARDWARE_INVALID             = -1,
    AML_HARDWARE_VSI_UNIFY           = 0,
    AML_HARDWARE_VIPLITE             = 1,
    AML_HARDWARE_ADLA                = 2,
    AML_HARDWARE_GPU                 = 3,
    AML_HARDWARE_CPU                 = 4,
    AML_HARDWARE_MAX                 = 100,
} aml_hw_type_t;

typedef struct {
    unsigned int core_num;
    unsigned int num;
    float flops;  // TOps
} aml_npu_hw_info_t;

typedef struct {
    unsigned int         cpuinfo;
    char*                sdk_version;
    char*                ddk_version;
    char*                hw_version;
    aml_hw_type_t        hw_type;
    aml_npu_hw_info_t    npu_hw_info;
    unsigned int         platform_type;
} aml_platform_info_t;

typedef struct {
    int32_t memory_size;    // memory size used by the loaded model
    int32_t axi_sram_size;  // axi sram size required by the loaded model
    int32_t num_layers;
    int64_t macc_count;
    double  sum_macc; //Gops
} aml_ctx_info_t;

typedef struct {
    int32_t working_frequency;      // in MHz
    int32_t axi_working_frequency;  // in MHz
    int32_t memory_size;            // memory size used by the device driver
    int32_t axi_sram_size;          // axi sram size allocated for the device
    float tops;                     // TOPS
} aml_dev_memory_info_t;

typedef struct {
    aml_ctx_info_t               ctx_info;
    aml_dev_memory_info_t        dev_info;
} aml_context_info_t;

typedef struct aml_profiling_ext_data
{
    uint64_t axi_freq_cur;   // adla axi clock frequency currently
    uint64_t core_freq_cur;  // adla core clock frequency currently
    uint64_t mem_alloced_base;
    uint64_t mem_alloced_umd;
    int64_t  mem_pool_size;  //-1:the limit base on the system
    uint64_t mem_pool_used;
    int32_t us_elapsed_in_hw_op;
    int32_t us_elapsed_in_sw_op;
} aml_profiling_ext_data_t;

typedef struct aml_profiling_data
{
    uint64_t inference_time_us;
    uint64_t memory_usage_bytes;
    uint64_t dram_read_bytes;
    uint64_t dram_write_bytes;
    uint64_t sram_read_bytes;
    uint64_t sram_write_bytes;
    aml_profiling_ext_data_t ext;
} aml_profiling_data_t;

typedef struct aml_work_frequency
{
    float                         *curr_freq;
    float                         *min_freq;
} aml_work_frequency_t;

typedef struct {
    unsigned int                   suspend_time;
    float                          freq;
    aml_profile_type_t             profile_type;
    aml_policy_type_t              policy_type;
    aml_profiling_data_t           profiling_data;
    aml_work_frequency_t           frequency_data;
    aml_context_info_t             context_info;
} aml_profile_config_t;

typedef enum {
    AML_WITH_MMU              = 0,
    AML_WITHOUT_MMU           = 1,
} aml_mmu_type_t;

typedef enum {
    AML_VIRTUAL_ADDR              = 0,
    AML_PHYS_ADDR                 = 1,
} aml_memory_type_t;

typedef  struct __aml_memory_data_t
{
    uint32_t                typeSize;
    void                   *memory;
    void                   *viraddr;
    uint64_t                phyaddr;
} aml_memory_data_t;

typedef enum {
    AML_MEM_DIRECTION_READ_WRITE  = 0,
    AML_MEM_DIRECTION_READ_ONLY   = 1,
    AML_MEM_DIRECTION_WRITE_ONLY  = 2,
} aml_mem_direction_t;

typedef  struct __aml_memory_config_t
{
    uint32_t               typeSize;
    uint32_t               mem_size;
    uint32_t               index;
    aml_mmu_type_t         mmu_type;
    aml_cache_type_t       cache_type;
    aml_memory_type_t      memory_type;
    aml_mem_direction_t    direction;
} aml_memory_config_t;


/*=============================================================
                     NNSDK main api
==============================================================*/
void*  aml_module_create(aml_config* config);                   /*==========create aml network module======*/
int  aml_module_input_set(void* context, nn_input *pInput);      /*==========set network input==============*/
void*  aml_module_output_get(void* context, aml_output_config_t outconfig); /*=======run and get output====*/
void*  aml_module_output_get_simple(void* context); /*=======run and get output simply,for custom network====*/
int  aml_module_destroy(void* context);      /*=====destroy network environment,free the alloced buffer====*/

/*============================================================
             some aml sdk util functions
==============================================================*/
unsigned char  *aml_util_mallocAlignedBuffer(void* context, int mem_size, aml_memory_config_t* mem_config);   /*======malloc 4k align buffer for dma IO====*/
void  aml_util_freeAlignedBuffer(void* context, unsigned char *addr);         /*======free buffer alloced by above=========*/
/*==swap input buffer,the inputId(for multi-number input)is ordered as aml_util_getInputTensorInfo array==*/
int  aml_util_swapInputBuffer(void *context, void *newBuffer, unsigned int inputId);
int  aml_util_swapOutputBuffer(void *context, void *newBuffer, unsigned int outputId);

/*=============    ddk version higher than 6.4.6.2.*.*.*   ===============*/
int  aml_util_switchInputBuffer(void *context, void *newBuffer, unsigned int inputId);
int  aml_util_switchOutputBuffer(void *context, void *newBuffer, unsigned int outputId);
int  aml_util_flushTensorHandle(void *context, aml_flush_type_t type);    /*=========== flush tensor cache memory =======================*/

/*=========== get tensor info from nbg files =======================*/
tensor_info*  aml_util_getInputTensorInfo(const char* nbgdata);  /*====get model input tensor information list=====*/
tensor_info*  aml_util_getOutputTensorInfo(const char* nbgdata); /*====get model output tensor information list====*/


int  aml_util_mallocBuffer(void* context, aml_memory_config_t* mem_config, aml_memory_data_t* mem_data);
int  aml_util_freeBuffer(void* context, aml_memory_config_t* mem_config, aml_memory_data_t* mem_data);
int  aml_util_flushBuffer(void* context, aml_memory_config_t* mem_config, aml_memory_data_t* mem_data);

int  aml_util_swapExternalInputBuffer(void *context, aml_memory_config_t* mem_config, aml_memory_data_t* mem_data);
int  aml_util_swapExternalOutputBuffer(void *context, aml_memory_config_t* mem_config, aml_memory_data_t* mem_data);

int  aml_util_getTensorInfo(void *context, const char* model_data, tensor_info** in_tInfo, tensor_info** out_tInfo);
int  aml_util_freeTensorInfo(tensor_info* tinfo);     /*====free the tensor_info memory get by above two functions*/

/*=========== get hardinfo from drivers and set profile =======================*/
int  aml_util_setProfile(aml_profile_type_t type, const char *savepath); /*===set profile type===*/
int  aml_util_setPowerPolicy(aml_policy_type_t type); /*===set power policy===*/
int  aml_util_getHardwareStatus(int* customID, int *powerStatus, int* version); /*===get hardware info===*/
int  aml_util_setAutoSuspend(int timeout);  /*===get hardware info===*/


int  aml_util_enableProfile(void *context, aml_profile_config_t* profile_data);
int  aml_util_getProfileInfo(void *context, aml_profile_config_t* profile_data);
int  aml_util_disableProfile(void *context, aml_profile_config_t* profile_data);

int  aml_read_chip_info(aml_platform_info_t* platform_info);

#ifdef __cplusplus
} //extern "C"
#endif

#endif // _NN_SDK_H