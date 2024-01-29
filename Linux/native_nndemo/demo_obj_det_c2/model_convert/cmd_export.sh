#!/bin/bash

#Meaning of related documents:
#   acuity_model_name.txt          --The name of model (eg. inceptionV1.pb)
#   acuity_inputs_outputs.txt      --The input and output ops (eg. --inputs input --outputs InceptionV1/Logits/SpatialSqueeze --input-size-list 224,224,3)
#   acuity_channel_mean_value.txt  --The channel and mean value (eg. 128 128 128 128)
#   acuity_quantized.txt           --The quantized file (eg.data/space_shuttle.jpeg)
#   acuity_validation.txt          --The validation file (eg.data/space_shuttle.jpeg)


NAME=det_yolov2
NPU_ID=$1
NBG_NAME=${NAME}_${NPU_ID}
VCMDTOOL_DIR=$(pwd)/../bin/vcmdtools

export ACUITY_PATH=$(pwd)/../bin/
pegasus=$ACUITY_PATH/pegasus

if [ ! -e "$pegasus" ]; then
    pegasus=$ACUITY_PATH/pegasus.py
fi

if (($1 == 0x7d));then
    CONFIG=VIPNANOQI_PID0X7D
    echo "CONFIG: ${CONFIG}"
elif (($1 == 0x88));then
    CONFIG=VIPNANOQI_PID0X88
    echo "CONFIG: ${CONFIG}"
elif (($1 == 0x99));then
    CONFIG=VIPNANOQI_PID0X99
    echo "CONFIG: ${CONFIG}"
elif (($1 == 0xa1));then
    CONFIG=VIPNANOQI_PID0XA1
    echo "CONFIG: ${CONFIG}"
elif (($1 == 0xb9));then
    CONFIG=VIPNANOQI_PID0XB9
    echo "CONFIG: ${CONFIG}"
elif (($1 == 0xbe));then
    CONFIG=VIPNANOQI_PID0XBE
    echo "CONFIG: ${CONFIG}"
elif (($1 == 0xe8));then
    CONFIG=VIPNANOQI_PID0XE8
    echo "CONFIG: ${CONFIG}"
elif (($1 == 0x1e));then
    CONFIG=VIPNANOQI_PID0X1E
    echo "CONFIG: ${CONFIG}"
else
    echo "ERROR: NO Npu Config"
    exit 1
fi

mkdir -p NB_${NPU_ID}
mkdir -p "${NAME}"


$pegasus export \
    ovxlib \
    --model ${NAME}.json \
    --model-data ${NAME}.data \
    --model-quantize ${NAME}.quantize \
    --dtype quantized \
    --optimize ${CONFIG} \
    --viv-sdk ${VCMDTOOL_DIR} \
    --with-input-meta ${NAME}_inputmeta.yml \
    --output-path ${NAME}/${NAME} \
    --pack-nbg-unify
    
mv  ${NAME}_nbg_unify  ${NBG_NAME}

mv ${NBG_NAME}/network_binary.nb ${NBG_NAME}/${NBG_NAME}.nb

cp -f ${NBG_NAME}/${NBG_NAME}.nb NB_${NPU_ID}
