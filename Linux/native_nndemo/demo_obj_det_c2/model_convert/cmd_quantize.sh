#!/bin/bash

NAME=det_yolov2

rm ${NAME}.quantize

export ACUITY_PATH=../bin
pegasus=$ACUITY_PATH/pegasus

if [ ! -e "$pegasus" ]; then
    pegasus=$ACUITY_PATH/pegasus
fi

$pegasus  quantize \
	--quantizer asymmetric_affine \
	--qtype uint8 \
	--rebuild \
	--with-input-meta  ${NAME}_inputmeta.yml \
	--model  ${NAME}.json \
	--model-data  ${NAME}.data