/*
 * GridPredictor.cpp
 *
 *  Created on: 05.10.2017
 *      Author: Christoph Neuhauser
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
    tf::graph::SetDefaultDevice("/cpu:0", &graphDef);

    // 3. Create session from loaded graph definition
    status = session->Create(graphDef);
    if (!status.ok()) {
        Logfile::get()->writeError(std::string() + "ERROR in GridPredictor::loadGraph: " + status.ToString());
        return false;
    }

    // 4. Initialize class member data
    inputTensor  = tf::Tensor(tf::DT_FLOAT, tf::TensorShape({1, DSC_IMG_SIZE, DSC_IMG_SIZE, 3}));
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


