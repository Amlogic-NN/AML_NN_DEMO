Introduction:
nnEngine is a model inference engine developed by Amlogic for Windows, based on the nnsdk development package. It facilitates data transmission and model execution control between embedded device terminals and servers using the ADB protocol. Users can perform model inference and acquire performance metrics through the provided aml_nn_engine Python library.

Usage Overview:
1. Model Conversion
2. Environment Configuration
3. nn_engine Usage Introduction

1. Model Conversion and Parameter Confirmation:
   - Model Conversion:
     Convert models from nnengine_demo/model to adla files for the current single board in a Linux environment using the model_convert.sh script. Note down the channel-mean-value (127.5,127.5,127.5,127.5).
   - Parameter Confirmation:
     After conversion, use check_input_param.sh to confirm zero_point/scale of the input/output for the adla file. Note the input_zero_point/input_scale.

2. Environment Configuration:
   - Windows PC Environment:
     Utilize the pyd package in nnengine_pc_37. Specify the path in the demo (e.g., sys.path.append('../nnengine_pc_37')).
   - Board-side Environment:
     1. Push nnEngine and adla files to /data/nn/nnEngine directory on the board.
     2. Start the board-side server:
        ```
        adb root
        adb remount
        adb shell mkdir /data/nn/
        adb push /xxxx/xxx.adla /data/nn/
        adb shell
        cd /data/nn/
        chmod +x nnEngine
        ./nnEngine
        ```

3. nn_engine Usage Introduction:
   - Invocation Process and Parameter Introduction:
     ```
     def get_image(path):
         img = Image.open(path).convert('RGB').resize((224,224))
         img=np.array(img)
         img=(img-127.5)/127.5
         data=np.expand_dims(img,axis=0)
         data=data/0.007843137718737125 +1
         data=np.array(data).astype(np.int8)
         return data
     ```

     ```
     from amlnn import engine
     amlnn = engine.AMLNN()
     amlnn.setloglevel("DEBUG")
     config=amlnn.get_config()
     config.device_type = 'android'
     config.model_type = 'adla'
     config.board_path='/data/vendor/nn'
     config.nb_path = './resnet18-v1-7_int8.adla'
     config.profile = 'all'
     config.hardware_info = '0'
     amlnn.init()
     img_name='./data/input/2242243.jpeg'
     config.input_size='224 224 3'
     config.input_type='raw'
     config.output_type='fp32'
     config.run_cyc=10
     config.input_data = get_image(img_name)
     amlnn.set_input()
     out=amlnn.inference()
     amlnn.destroy()
     ```

     Note:
     - Ensure dependencies for demo.py/demo_single.py/demo_multi_io.py on PC side are configured.
     - For multi-input and multi-output models, refer to demo_multi_io.py.
     - For testing datasets, refer to demo.py.