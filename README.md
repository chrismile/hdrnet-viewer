# hdrnet-viewer

In 2017, Gharbi et al. published the paper 'Deep Bilateral Learning for Real-Time Image Enhancement' in SIGGRAPH 2017 (vol 36).
The paper is about reconstructing image filters and human retouchment using deep neural networks.
An implementation of the neural network architecture using Tensorflow 1.1 can be found at: https://github.com/mgharbi/hdrnet

This application is a webcam viewer application using OpenGL 4.3 and the TensorFlow C++ API for network models learned by
that network architecture implementation. It is in no way endorsed by Gharbi et al.


## Building and running the programm

Prerequisites (build currently only tested on Ubuntu 18.04):
- OpenCV 2
- Boost

- TensorflowCC C++ API headers & library (tested: version 1.1 and 2.2)
  
  The standard Python package of Tensorflow is not sufficient to build the application!
  You can read more about compiling the Tensorflow C++ API here: https://github.com/FloopCZ/tensorflow_cc
  
- sgl: https://github.com/chrismile/sgl (recommended to use 'sudo make install' to install this library on your system)

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


## TensorflowCC

If you wish to install TensorflowCC to a custom location, use e.g. the following command for compiling TensorflowCC.

```
cmake -DCMAKE_INSTALL_PREFIX:PATH=~/build/tensorflow_cc_install -DALLOW_CUDA=OFF ..
```

Then, configure hdrnet-viewer with the following command.

```
cmake -DTensorflowCC_DIR=/home/christoph/build/tensorflow_cc_install/lib/cmake/TensorflowCC/ ..
```

Note: As of writing, bazel-2.0.0 is necessary for building TensorflowCC. Add the folder containing a script called
'bazel' with the following content to the PATH variable.

```
#!/bin/bash
bazel-2.0.0 "$@"
```
