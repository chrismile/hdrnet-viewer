/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2017, Christoph Neuhauser
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

#if (CV_VERSION_MAJOR <= 2)
    bool b1 = stream->set(CV_CAP_PROP_FRAME_WIDTH, 640);
    bool b2 = stream->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
#else
    bool b1 = stream->set(cv::CAP_PROP_FRAME_WIDTH, 640);
    bool b2 = stream->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
#endif

    //std::cout << "Camera resolution width is " << stream->get(cv::CAP_PROP_FRAME_WIDTH) << std::endl;
    //std::cout << "Camera resolution width is " << stream->get(cv::CAP_PROP_FRAME_WIDTH) << " | change is ok: " << ((int)b1) << std::endl;

    return true;
}

bool Webcam::readFrame(FrameDataPtr frameImage, FrameDataPtr downscaledImage) {
    cv::Mat frame;
    if (!stream->read(frame)) {
        // No frame to be read
        return false;
    }

    if (frameImage->pixels == 0) {
        frameImage->pixels = new uchar[frame.total()*4];
        frameImage->w = frame.cols;
        frameImage->h = frame.rows;
        downscaledImage->pixels = new uchar[256*256*4];
        downscaledImage->w = 256;
        downscaledImage->h = 256;
    }

    cv::Mat rgbaMat(frame.size(), CV_8UC4, frameImage->pixels);
#if (CV_VERSION_MAJOR <= 2)
    cv::cvtColor(frame, rgbaMat, CV_BGR2RGBA, 4);
#else
    cv::cvtColor(frame, rgbaMat, cv::COLOR_BGR2RGBA, 4);
#endif

    // Downscale
    cv::Mat downscaledMat(256, 256, CV_8UC4, downscaledImage->pixels);
    cv::resize(rgbaMat, downscaledMat, cv::Size(256, 256), 0, 0, cv::INTER_AREA); // INTER_LINEAR INTER_CUBIC INTER_AREA

    return true;
}

glm::ivec2 Webcam::getResolution() {
#if (CV_VERSION_MAJOR <= 2)
    int cameraWidth = stream->get(CV_CAP_PROP_FRAME_WIDTH);
    int cameraHeight = stream->get(CV_CAP_PROP_FRAME_HEIGHT);
#else
    int cameraWidth = stream->get(cv::CAP_PROP_FRAME_WIDTH);
    int cameraHeight = stream->get(cv::CAP_PROP_FRAME_HEIGHT);
#endif
    return glm::ivec2(cameraWidth, cameraHeight);
}
