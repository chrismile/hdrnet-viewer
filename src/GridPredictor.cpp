/*
 * Code for handling input files from: https://github.com/mgharbi/hdrnet/blob/master/benchmark/src/processor.cc
 *
 * Copyright 2016 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * License of other parts of the code:
 *
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
 */

#include "GridPredictor.hpp"
#include <Utils/File/Logfile.hpp>

// Downscaled image width/heigth
const int DSC_IMG_SIZE = 256;
const std::string outputName = "output_coefficients";
const std::string inputName = "lowres_input";
const std::string graphFilename = "frozen_graph.pb";

using namespace sgl;

GridPredictor::GridPredictor() : session(NULL) {
}

GridPredictor::~GridPredictor() {
    if (session) {
        session->Close();
    }
}

bool GridPredictor::loadGraph(const char *path) {
    // 1. Create Tensorflow session
    tf::Status status = tf::NewSession(tf::SessionOptions(), &session);

    if (!status.ok()) {
        Logfile::get()->writeError(std::string() + "ERROR in GridPredictor::loadGraph: " + status.ToString());
        return false;
    }

    // 2. Load network graph definition from file
    status = tf::ReadBinaryProto(tf::Env::Default(), std::string() + path + graphFilename, &graphDef);
    if (!status.ok()) {
        Logfile::get()->writeError(std::string() + "ERROR in GridPredictor::loadGraph: " + status.ToString());
        return false;
    }
    //tf::graph::SetDefaultDevice("/cpu:0", &graphDef);

    // 3. Create session from loaded graph definition
    status = session->Create(graphDef);
    if (!status.ok()) {
        Logfile::get()->writeError(std::string() + "ERROR in GridPredictor::loadGraph: " + status.ToString());
        return false;
    }

    // 4. Initialize class member data
    inputTensor = tf::Tensor(tf::DT_FLOAT, tf::TensorShape({1, DSC_IMG_SIZE, DSC_IMG_SIZE, 3}));
    inputs = {{inputName, inputTensor}};
    status = session->Run(inputs, {outputName}, {}, &outputs);
    gridSize = glm::ivec3(outputs[0].dim_size(3), outputs[0].dim_size(2), outputs[0].dim_size(1));

    return true;
}

float *GridPredictor::computeGridCoefficients(FrameDataPtr &lowresImage) {
    float* inputPixels = inputTensor.flat<float>().data();
    for (int y = 0; y < DSC_IMG_SIZE; ++y) {
        for (int x = 0; x < DSC_IMG_SIZE; ++x) {
            // 3 color channels
            for (int c = 0; c < 3; ++c) {
                inputPixels[c+3*(x+DSC_IMG_SIZE*y)] = lowresImage->pixels[c+4*(x+DSC_IMG_SIZE*y)]/255.0f;
            }
        }
    }

    tf::Status status = session->Run(inputs, {outputName}, {}, &outputs);
    if (!status.ok()) {
        Logfile::get()->writeError(std::string() + "ERROR in GridPredictor::loadGraph: " + status.ToString());
        return NULL;
    }

    // Affine transform coefficients
    float* coefficientData = outputs[0].flat<float>().data();
    return coefficientData;
}


