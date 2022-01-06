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


#include <string>
#include <fstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <Utils/File/Logfile.hpp>
#include <Graphics/Shader/ShaderAttributes.hpp>
#include <Graphics/Shader/ShaderManager.hpp>
#include <Graphics/Window.hpp>
#include <Utils/AppSettings.hpp>
#include <Math/Math.hpp>
#include "GridRenderer.hpp"

using namespace sgl;

void loadBytesFromFile(const std::string& filename, int bytes, char *buffer) {
    std::ifstream file(filename.c_str(), std::ios::binary);
    if(!file.is_open()) {
        Logfile::get()->writeError(
                std::string() + "ERROR in loadFileContent: Couldn't load file \"" + filename + "\".");
    }
    file.read(buffer, bytes);
    file.close();
}

GridRenderer::GridRenderer() {
    gridRenderShader = ShaderManager->getShaderProgram(
            {"ApplyCoefficients.Vertex", "ApplyCoefficients.Fragment"});
    blitShader = ShaderManager->getShaderProgram(
            {"Blit.Vertex", "Blit.Fragment"});
}

void GridRenderer::initialize(const std::string& path) {
    gridPredictor = GridPredictor();
    gridPredictor.loadGraph(path);
    glm::ivec3 gridSize = gridPredictor.getGridSize();

    TextureSettings settings;
    settings.type = TEXTURE_3D;
    settings.internalFormat = GL_RGBA16F;
    gridTextures.clear();
    for (int i = 0; i < 3; ++i) {
        TexturePtr gridTexture = TextureManager->createEmptyTexture(
                gridSize.x, gridSize.y, gridSize.z, settings);
        gridTextures.push_back(gridTexture);
    }

    loadGuideParameters(path);
}

AABB2 getRenderRect(sgl::TexturePtr &imageTexture) {
    Window *window = AppSettings::get()->getMainWindow();

    AABB2 renderRect;
    glm::vec2 extent;
    float windowRatio = float(window->getWidth())/window->getHeight();
    float imageRatio = float(imageTexture->getW()) / imageTexture->getH();
    if (windowRatio >= imageRatio) {
        extent = glm::vec2(imageRatio/windowRatio, 1.0f);
    } else {
        extent = glm::vec2(1.0f, windowRatio/imageRatio);
    }
    renderRect.max = extent;
    renderRect.min = -extent;

    return renderRect;
}


void GridRenderer::renderTransformedImage(sgl::TexturePtr &imageTexture, FrameDataPtr &lowresImage) {
    // Set-up the vertex data of the rectangle
    AABB2 renderRect = getRenderRect(imageTexture);
    std::vector<VertexTextured> fullscreenQuad(createTexturedQuad(renderRect));

    // Feed the shader with the data and render the quad
    int stride = sizeof(VertexTextured);
    GeometryBufferPtr geomBuffer = Renderer->createGeometryBuffer(
            sizeof(VertexTextured)*fullscreenQuad.size(), &fullscreenQuad.front());
    sgl::ShaderAttributesPtr gridRenderData = ShaderManager->createShaderAttributes(gridRenderShader);
    gridRenderData->addGeometryBuffer(
            geomBuffer, "vertexPosition", ATTRIB_FLOAT, 3, 0, stride);
    gridRenderData->addGeometryBuffer(
            geomBuffer, "vertexTexCoord", ATTRIB_FLOAT, 2, sizeof(glm::vec3), stride);


    float *affineCoefficients = gridPredictor.computeGridCoefficients(lowresImage);
    glm::ivec3 gridSize = gridPredictor.getGridSize();

    gridRenderShader->setUniform("image", imageTexture, 0);
    for (int i = 0; i < 3; ++i) {
        float *data = affineCoefficients + i*gridSize.x*gridSize.y*gridSize.z*4;
        TexturePtr gridTexture = gridTextures[i];
        gridTexture->uploadPixelData(
                gridSize.x, gridSize.y, gridSize.z, data,
                PixelFormat(GL_RGBA, GL_FLOAT));
        std::string texUniformName = std::string() + "affineGridRow" + sgl::toString(i);
        gridRenderShader->setUniform(texUniformName.c_str(), gridTexture, i+1);
    }

    Renderer->render(gridRenderData);
}

void GridRenderer::renderNormalImage(sgl::TexturePtr &imageTexture, FrameDataPtr &lowresImage) {
    // Set-up the vertex data of the rectangle
    AABB2 renderRect = getRenderRect(imageTexture);
    std::vector<VertexTextured> fullscreenQuad(createTexturedQuad(renderRect));

    // Feed the shader with the data and render the quad
    int stride = sizeof(VertexTextured);
    GeometryBufferPtr geomBuffer = Renderer->createGeometryBuffer(
            sizeof(VertexTextured)*fullscreenQuad.size(), &fullscreenQuad.front());
    sgl::ShaderAttributesPtr renderData = ShaderManager->createShaderAttributes(blitShader);
    renderData->addGeometryBuffer(
            geomBuffer, "vertexPosition", ATTRIB_FLOAT, 3, 0, stride);
    renderData->addGeometryBuffer(
            geomBuffer, "vertexTexCoord", ATTRIB_FLOAT, 2, sizeof(glm::vec3), stride);
    blitShader->setUniform("inputTexture", imageTexture);

    Renderer->render(renderData);
}

// See https://github.com/mgharbi/hdrnet/blob/master/benchmark/src/renderer.cc for more details
void GridRenderer::loadGuideParameters(const std::string& path) {
    glm::mat3x4 ccm;
    glm::vec4 mixMatrix;
    glm::vec3 shifts[16];
    glm::vec3 slopes[16];

    loadBytesFromFile(
            std::string() + path + "guide_ccm_f32_3x4.bin", sizeof(glm::mat3x4), (char*)&ccm);
    loadBytesFromFile(
            std::string() + path + "guide_mix_matrix_f32_1x4.bin", sizeof(glm::vec4), (char*)&mixMatrix);
    loadBytesFromFile(
            std::string() + path + "guide_shifts_f32_16x3.bin", 16*3*sizeof(float), (char*)shifts);
    loadBytesFromFile(
            std::string() + path + "guide_slopes_f32_16x3.bin", 16*3*sizeof(float), (char*)slopes);

    gridRenderShader->setUniform("guideCCM", ccm);
    gridRenderShader->setUniform("mixMatrix", mixMatrix);
    gridRenderShader->setUniformArray("guideShifts", shifts, 16);
    gridRenderShader->setUniformArray("guideSlopes", slopes, 16);
}


std::vector<VertexTextured> GridRenderer::createTexturedQuad(const AABB2 &renderRect) {
    glm::vec2 min = renderRect.getMinimum();
    glm::vec2 max = renderRect.getMaximum();
    std::vector<VertexTextured> quad{
            VertexTextured(glm::vec3(max.x,max.y,0), glm::vec2(0, 0)),
            VertexTextured(glm::vec3(min.x,min.y,0), glm::vec2(1, 1)),
            VertexTextured(glm::vec3(max.x,min.y,0), glm::vec2(0, 1)),
            VertexTextured(glm::vec3(min.x,min.y,0), glm::vec2(1, 1)),
            VertexTextured(glm::vec3(max.x,max.y,0), glm::vec2(0, 0)),
            VertexTextured(glm::vec3(min.x,max.y,0), glm::vec2(1, 0))};
    return quad;
}
