/*
 * GridPredictor.hpp
 *
 *  Created on: 05.10.2017
 *      Author: Christoph Neuhauser
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
