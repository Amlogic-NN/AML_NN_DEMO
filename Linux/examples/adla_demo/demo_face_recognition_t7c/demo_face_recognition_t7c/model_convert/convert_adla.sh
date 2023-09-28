#!/bin/bash
  
ACUITY_PATH=../bin/
#ACUITY_PATH=../python/tvm/
adla_convert=${ACUITY_PATH}adla_convert


if [ ! -e "$adla_convert" ]; then
    adla_convert=${ACUITY_PATH}adla_convert.py
fi


$adla_convert --model-type tensorflow \
        --model ./model_source/tensorflow_model/det_insightface.pb \
        --inputs data --input-shape 112,112,3 \
        --outputs l2_normalize \
        --quantize-dtype int8 --outdir tensorflow_output \
        --channel-mean-value "128,128,128,128" \
        --source-file dataset.txt \
		--iterations 10 \
        --target-platform PRODUCT_PID0XA003