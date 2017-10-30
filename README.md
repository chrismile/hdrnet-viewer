# hdrnet-viewer

In 2017, Gharbi et al. published the paper 'Deep Bilateral Learning for Real-Time Image Enhancement' in SIGGRAPH 2017 (vol 36).
The paper is about reconstructing image filters and human retouchment using deep neural networks.
An implementation of the neural network architecture can be found at: https://github.com/mgharbi/hdrnet
This application is a webcam viewer application for network models learned by that network architecture implementation using Tensorflow 1.1. It is in no way endorsed by Gharbi et al.

Prerequisites (build currently only supported on Linux):
- OpenCV 2
- Tensorflow 1.1 C++ API headers & library, expected to be in installed in the directory /usr/local
- sgl: https://github.com/chrismile/sgl
