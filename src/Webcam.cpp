/*
 * Webcam.cpp
 *
 *  Created on: 04.10.2017
 *      Author: Christoph Neuhauser
 */

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <Utils/File/Logfile.hpp>
#include "Webcam.hpp"

using namespace sgl;

FrameData::FrameData() : pixels(NULL), w(0), h(0) {
}

FrameData::~FrameData() {
    if (pixels) {
        delete pixels;
    }
}

Webcam::Webcam() : stream(NULL) {
}

Webcam::~Webcam() {
    if (stream) {
        delete stream;
    }
}

bool Webcam::open(int id) {
    stream = new cv::VideoCapture(id);

    if (!stream->isOpened()) {
        Logfile::get()->writeError("ERROR: Can't open webcam");
        return false;
    }

    //bool b1 = stream->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    //bool b2 = stream->set(CV_CAP_PROP_FRAME_HEIGHT, 720);
    //std::cout << "Camera resolution width is " << stream->get(CV_CAP_PROP_FRAME_WIDTH) << " | change is ok: " << ((int)b1) << std::endl;

    return true;
}

bool Webcam::readFrame(FrameDataPtr frameImage, FrameDataPtr downscaledImage) {
    cv::Mat frame;
    if (!stream->read(frame)) {
        // No frame to be read
        return false;
    }

    if (frameImage->pixels == 0) {
        frameImage->pixels = new uchar[frame.total()*4];;
        frameImage->w = frame.cols;
        frameImage->h = frame.rows;
        downscaledImage->pixels = new uchar[256*256*4];;
        downscaledImage->w = 256;
        downscaledImage->h = 256;
    }

    cv::Mat rgbaMat(frame.size(), CV_8UC4, frameImage->pixels);
    cv::cvtColor(frame, rgbaMat, CV_BGR2RGBA, 4);

    // Downscale
    cv::Mat downscaledMat(256, 256, CV_8UC4, downscaledImage->pixels);
    cv::resize(rgbaMat, downscaledMat, cv::Size(256, 256), 0, 0, cv::INTER_AREA); // INTER_LINEAR INTER_CUBIC INTER_AREA

    return true;
}


