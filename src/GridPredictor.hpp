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

#ifndef GRIDPREDICTOR_HPP_
#define GRIDPREDICTOR_HPP_

#include <glm/glm.hpp>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/graph/graph.h>
#include <tensorflow/core/graph/default_device.h>
#include "FrameData.hpp"

namespace tf = tensorflow;

//! Used to compute the bilateral grid containing affine transform coefficients
class GridPredictor {
public:
    GridPredictor();
    ~GridPredictor();
    //! \param path: Path to folder containing graph data
    bool loadGraph(const char *path);
    /*!
     * \param lowresImage: 256x256 32-bit RGBA image
     * \return Returns the affine transform coefficients stored in the grid
     */
    float *computeGridCoefficients(FrameDataPtr &lowresImage);

    // Getters
    glm::ivec3 getGridSize() { return gridSize; }

private:
    tensorflow::Session *session;
    tensorflow::GraphDef graphDef;
    tf::Tensor inputTensor;
    std::vector<std::pair<std::string, tf::Tensor>> inputs;
    std::vector<tf::Tensor> outputs;
    glm::ivec3 gridSize;
};


#endif /* GRIDPREDICTOR_HPP_ */
