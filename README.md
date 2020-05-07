# hdrnet-viewer

In 2017, Gharbi et al. published the paper 'Deep Bilateral Learning for Real-Time Image Enhancement' in SIGGRAPH 2017 (vol 36).
The paper is about reconstructing image filters and human retouchment using deep neural networks.
An implementation of the neural network architecture using Tensorflow 1.1 can be found at: https://github.com/mgharbi/hdrnet

This application is a webcam viewer application using OpenGL and the TensorFlow C++ API for network models learned by that network architecture implementation. It is in no way endorsed by Gharbi et al.

## Building and running the programm

Prerequisites (build currently only supported on Linux):
- OpenCV 2
- Tensorflow 1.1 C++ API headers & library, expected to be in installed in the directory /usr/local

  The standard Python package of Tensorflow is not sufficient to build the application!
  You can read more about compiling the Tensorflow C++ API here: https://github.com/FloopCZ/tensorflow_cc
  
  Please note that the _shared_ build of TensorflowCC is required, as the application uses broad parts of the TensorFlow C++ API.
  
- sgl: https://github.com/chrismile/sgl (use sudo make install to install this library on your system)

Please note that pre-learned image filters are not included in this repository. The original implementation contains multiple pre-learned models. Please get them from https://github.com/mgharbi/hdrnet and put them into the folder "Data/pretrained_models".

After installing sgl (see above) execute in the repository directory:

```
mkdir build
cd build
cmake ..
make -j
ln -s ../Data .
```
(Alternatively, use 'cp -R ../Data .' to copy the Data directory instead of creating a soft link to it).
