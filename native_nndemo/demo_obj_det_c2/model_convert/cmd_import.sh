#!/bin/bash

MODEL_NAME=det_yolov2

rm ${MODEL_NAME}.json ${MODEL_NAME}.data ${MODEL_NAME}.quantize

export ACUITY_PATH=$(pwd)/../bin/
pegasus=${ACUITY_PATH}pegasus

if [ ! -e "$pegasus" ]; then
    pegasus=${ACUITY_PATH}pegasus
fi


# If option '--weights' is miss, acuity will auto generate a fakedata for this model
$pegasus import darknet\
    --model  ./model/darknet/${MODEL_NAME}.cfg \
	--weights  ./model/darknet/${MODEL_NAME}.weights \
    --output-model ${MODEL_NAME}.json \
    --output-data ${MODEL_NAME}.data \
    

$pegasus generate \
    inputmeta \
    --model ${MODEL_NAME}.json \
    --input-meta-output ${MODEL_NAME}_inputmeta.yml \
    --channel-mean-value "0 0 0 0.00392156" \
    --source-file dataset.txt

# mv ${MODEL_NAME}.json ${MODEL_NAME}.data ${MODEL_NAME}_inputmeta.yml ./case/${NAME}

#mv ${MODEL_NAME}.json ${MODEL_NAME}.data ./case/${NAME}
