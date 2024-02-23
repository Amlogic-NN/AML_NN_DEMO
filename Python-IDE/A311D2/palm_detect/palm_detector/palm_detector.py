import cv2
import numpy as np
import tensorflow as tf
import math

from amlnn import engine
#from aml_nn_engine.amlnn import AMLNN
# Load the BLAZE_PALM_ANCHORS data from a text file


file_name = './anchors.txt'
with open(file_name, 'r') as file:
    line = file.readline()
    BLAZE_PALM_ANCHORS = [float(value) for value in line.strip().split(',')]

#Read and preprocess the image
def preprocess_image(img_name,target_size=(192,192)):
    # Read the image
    original_image = cv2.imread(img_name)
    #Convert BGR to RGB
    original_image_rgb = cv2.cvtColor(original_image,cv2.COLOR_BGR2RGB)
    #Resize the image to the target size
    input_image = cv2.resize(original_image_rgb, target_size)
    #Convert the image to a float32 Numpy array and normalize
    processed_input = np.asarray(input_image, dtype=np.float32) / 255.0
    return processed_input, original_image_rgb.shape[:2]

# Save preprocessed input data to binary file
#def save_output_to_bin(preprocessed_input, bin_file_path):
#    with open(bin_file_path, 'wb') as file:
#        # Save input data as binary
#        file.write(preprocessed_input.tobytes())


def save_preprocess_to_bin(preprocessed_input, bin_file_path):
    with open(bin_file_path, 'wb') as file:
        # Save input data as binary
        file.write(preprocessed_input.tobytes())


def test_model():

    amlnn = engine.AMLNN()

    amlnn.setloglevel("WARNING")                     #Valid values['DEBUG','INFO','WARNING],default:INFO

    #set init config
    config = amlnn.get_config()
    config.device_type = 'android'  # Valid values['android','linux'],default:'android'
    config.model_type = 'adla'
    config.board_path='/data/vendor/nn'                   # Working path on the board
   

    config.nb_path = '/data/vendor/nn/palm_detect_int8.adla'  # The adla file path in the borard: /data/nn/resnet18-v1-7_int8.adla

    config.profile = '0'  # default: all, Valid values['0','bw','runtime','all'], if set value all and setloglevel 'DEBUG', bw and runtime information will be printed in the log.
    config.hardware_info = '0'  # default: all, Valid values['0','all'], if set value all and setloglevel 'DEBUG', Chip information will be printed in the log

    #set init config
    amlnn.init()

    # set input config
    img_name = './data/hand.jpg'
    config.input_size = '192 192 3' #input shape info
    config.input_type = 'raw'  # input_type info, default:raw, Valid values['raw', 'dma'], Both require input_data to provide quantified data, and the corresponding input copy times are different.
    config.output_type = 'fp32'  # output_type info, default:fp32, Valid values['fp32','raw'], raw: Quantitative data, fp32: float32 result
    config.run_cyc = 1  # default: 1,  Call inference, internal execution times.Used to count single frame inference time
    processed_input,(original_height, original_width)= preprocess_image(img_name)  # set_input from memory
    print(original_height, original_width)
    


    config.input_data = processed_input # set_input from memory
    


    amlnn.set_input()

    # inference
    out = amlnn.inference()
    print(out.shape)

    output_data_scores = out[:(1*2016*1)].reshape((1, 2016, 1))
    print(output_data_scores.shape)
    

    output_data_boxes = out[(1*2016*1):(1*2016*1+1*2016*18)].reshape((1, 2016, 18))
    print(output_data_boxes.shape)
    
    #Define ObjectInfo class
    class ObjectInfo:
        def __init__(self):
            self.score = 0.0
            self.rect = {}

    def sigmoid(x):
        return  1 / (1 + math.exp(-x))

    #Define a function to parse outputs
    def parse_outputs(boxes, scores, score_thresh):
        objects = []
        batch, num_boxes, _= boxes.shape
        for i in range(num_boxes):
            score = sigmoid(scores[0, i, 0])
            if score < score_thresh:
                continue

            #Calculate offset using BLAZE_PALM_ANCHORS
            offset_x = BLAZE_PALM_ANCHORS[4 * i + 0] * 192
            offset_y = BLAZE_PALM_ANCHORS[4 * i + 1] * 192

            box = boxes[0, i, :]
            object_info = ObjectInfo()
            object_info.score = score

            # Parse the box information
            cx = box[0] + offset_x
            cy = box[1] + offset_y
            w = box[2]
            h = box[3]


            scale_x  = original_width / 192
            scale_y  = original_height / 192

            # Ensure the detection box is a square
            max_side = max(w, h)
            object_info.rect = {
                'top': int((cy -0.5 *max_side) * scale_y),
                'left': int((cx -0.5 * max_side) * scale_x),
                'bottom': int((cy + 0.5 * max_side) * scale_y),
                'right': int((cx + 0.5 * max_side) * scale_x)
            }

            objects.append(object_info)

        return objects

    #Parse the outputs with an  adjusted threshold
    objects = parse_outputs(output_data_boxes, output_data_scores, score_thresh=0.1)

    #Convert rect values to numpy array for cv2.dnn.NMSBoxes
    rects = np.array([[obj.rect['left'], obj.rect['top'], obj.rect['right'], obj.rect['bottom']] for obj in objects])

    #Perform NMS using cv2.dnn.NMSBoxes
    nms_threshold = 0.3   #Adjust this threshold as needed
    selected_indices = cv2.dnn.NMSBoxes(rects.tolist(), [obj.score for obj in objects], 0.0, nms_threshold)

    #Filter objects based on selected indices
    selected_objects = [objects[i[0]] for i in selected_indices]

    #Debugging:print the number of selected objects after NMS
    print("Selected objects after NMS:", len(selected_indices))

    #Draw the delected objects on the output image
    original_image = cv2.imread(img_name)
    output_image = original_image.copy()

    for obj in selected_objects:
        # Print each object's score and position for debugging

        print("Score", obj.score)
        print("Rect:", obj.rect)
        max_side = max(obj.rect['right'] - obj.rect['left'], obj.rect['bottom'] - obj.rect['top'])
        #Draw bounding box
        cv2.rectangle(output_image, (obj.rect['left'], obj.rect['top']),
                      (obj.rect['right'], obj.rect['bottom']), (0, 255, 0), 2)

    cv2.imwrite('output_hand_adla_version_2_0_hand_debug.jpg', output_image)

    amlnn.destroy()

if __name__ == "__main__":
    test_model()
