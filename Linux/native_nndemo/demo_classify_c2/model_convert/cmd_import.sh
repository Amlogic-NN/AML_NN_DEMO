#!/bin/bash

MODEL_NAME=sdk_mobilenetv1

rm ${MODEL_NAME}.json ${MODEL_NAME}.data ${MODEL_NAME}.quantize

export ACUITY_PATH=$(pwd)/../bin/
pegasus=${ACUITY_PATH}pegasus

if [ ! -e "$pegasus" ]; then
    pegasus=${ACUITY_PATH}pegasus
fi


# If option '--weights' is miss, acuity will auto generate a fakedata for this model
$pegasus import \
    tensorflow \
    --model ./model/tensorflow/sdk_mobilenetv1.pb \
    --inputs input \
    --outputs 'MobilenetV1/Logits/SpatialSqueeze' \
    --output-model ${MODEL_NAME}.json \
    --output-data ${MODEL_NAME}.data \
    --input-size-list '224,224,3'
    

$pegasus generate \
    inputmeta \
    --model ${MODEL_NAME}.json \
    --input-meta-output ${MODEL_NAME}_inputmeta.yml \
    --channel-mean-value "128 128 128 0.0078125" \
    --source-file dataset.txt

# mv ${MODEL_NAME}.json ${MODEL_NAME}.data ${MODEL_NAME}_inputmeta.yml ./case/${NAME}

#mv ${MODEL_NAME}.json ${MODEL_NAME}.data ./case/${NAME}
