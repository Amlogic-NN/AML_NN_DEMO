#!/bin/bash
  
ACUITY_PATH=../bin/
#ACUITY_PATH=../python/tvm/
adla_convert=${ACUITY_PATH}adla_convert


if [ ! -e "$adla_convert" ]; then
    adla_convert=${ACUITY_PATH}adla_convert.py
fi


$adla_convert --model-type tensorflow \
        --model ./model_source/tensorflow_model/det_retinaface.pb \
        --inputs data --input-shape 384,640,3 \
        --outputs "face_rpn_cls_prob_stride32 add_17 add_18 face_rpn_cls_prob_stride16 add_25 add_26 face_rpn_cls_prob_stride8 add_29 add_30" \
        --quantize-dtype int8 --outdir tensorflow_output \
        --channel-mean-value "128,128,128,1" \
        --source-file dataset.txt \
		--iterations 10 \
        --target-platform PRODUCT_PID0XA001