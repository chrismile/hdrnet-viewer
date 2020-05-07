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

#include "MainApp.hpp"

#include <ImGui/ImGuiWrapper.hpp>
#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_custom.h>
#include <ImGui/imgui_stdlib.h>

#include <Input/Keyboard.hpp>
#include <Math/Math.hpp>
#include <Math/Geometry/MatrixUtil.hpp>
#include <Graphics/Window.hpp>
#include <Utils/AppSettings.hpp>
#include <Utils/Events/EventManager.hpp>
#include <Utils/Timer.hpp>
#include <Input/Mouse.hpp>
#include <Input/Keyboard.hpp>
#include <Input/Gamepad.hpp>
#include <Utils/File/Logfile.hpp>
#include <Utils/File/FileUtils.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/Shader/ShaderManager.hpp>
#include <Graphics/Texture/Bitmap.hpp>
#include <GL/glew.h>
#include <climits>

void openglErrorCallback() {
    std::cerr << "Application callback" << std::endl;
}

MainApp::MainApp() {
    sgl::EventManager::get()->addListener(sgl::RESOLUTION_CHANGED_EVENT,
            [this](sgl::EventPtr event){ this->resolutionChanged(event); });
    sgl::Renderer->setErrorCallback(&openglErrorCallback);
    sgl::Renderer->setDebugVerbosity(sgl::DEBUG_OUTPUT_CRITICAL_ONLY);

    // Webcam data
    webcam.open();
    frameImage = FrameDataPtr(new FrameData);
    downscaledImage = FrameDataPtr(new FrameData);

    filters = {
            //"Data/pretrained_models/photoshop/instagram/",
            "Data/pretrained_models/photoshop/eboye/",
            "Data/pretrained_models/faces/",
            //"Data/pretrained_models/style_transfer/style_transfer_1024/",
            //"Data/pretrained_models/style_transfer/style_transfer_2048/",
            ///////"Data/pretrained_models/style_transfer/style_transfer_n/",
            "Data/pretrained_models/local_laplacian/normal_1024/",
            "Data/pretrained_models/local_laplacian/strong_1024/",
            //"Data/pretrained_models/photoshop/early_bird/",
            //"Data/pretrained_models/photoshop/false_colors/",
            //"Data/pretrained_models/photoshop/infrared/",
            //"Data/pretrained_models/photoshop/lomo_fi/",
    };
    filterNames = {
            //"Instagram",
            "Eboye",
            "Faces",
            //"Style Transfer 1024",
            //"Style Transfer 2048",
            "Normal 1024",
            "Strong 1024",
            //"Early Bird",
            //"False Colors",
            //"Infrared",
            //"lomo_fi",
    };
    filterIndex = 0;

    // Filter
    gridRenderer.initialize(filters[filterIndex].c_str());
}

MainApp::~MainApp() {
}

void MainApp::resolutionChanged(sgl::EventPtr event) {
    sgl::Window *window = sgl::AppSettings::get()->getMainWindow();
    glViewport(0, 0, window->getWidth(), window->getHeight());
}

void MainApp::processSDLEvent(const SDL_Event &event) {
    sgl::ImGuiWrapper::get()->processSDLEvent(event);
}

void MainApp::render() {
    sgl::Window *window = sgl::AppSettings::get()->getMainWindow();
    glViewport(0, 0, window->getWidth(), window->getHeight());

    if (webcam.readFrame(frameImage, downscaledImage)) {
        if (!frameTexture) {
            frameTexture = sgl::TextureManager->createEmptyTexture(frameImage->w, frameImage->h);
            downscaledTexture = sgl::TextureManager->createEmptyTexture(downscaledImage->w, downscaledImage->h);
        }
        frameTexture->uploadPixelData(frameImage->w, frameImage->h, frameImage->pixels);
        downscaledTexture->uploadPixelData(downscaledImage->w, downscaledImage->h, downscaledImage->pixels);
    }

    glm::mat4 newProjMat(sgl::matrixOrthogonalProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f));
    sgl::Renderer->setProjectionMatrix(newProjMat);
    sgl::Renderer->setViewMatrix(sgl::matrixIdentity());
    sgl::Renderer->setModelMatrix(sgl::matrixIdentity());

    sgl::Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, sgl::Color(0, 0, 0));

    if (frameTexture) {
        if (sgl::Keyboard->isKeyDown(SDLK_SPACE)) {
            gridRenderer.renderNormalImage(frameTexture, downscaledImage);
        } else {
            gridRenderer.renderTransformedImage(frameTexture, downscaledImage);
        }

        sgl::Renderer->errorCheck();
    }

    renderGUI();
}

void MainApp::renderGUI() {
    sgl::ImGuiWrapper::get()->renderStart();

    if (showSettingsWindow) {
        if (ImGui::Begin("Settings", &showSettingsWindow)) {
            // Draw an FPS counter
            static float displayFPS = 60.0f;
            static uint64_t fpsCounter = 0;
            if (sgl::Timer->getTicksMicroseconds() - fpsCounter > 1e6) {
                displayFPS = ImGui::GetIO().Framerate;
                fpsCounter = sgl::Timer->getTicksMicroseconds();
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);
            ImGui::Separator();

            // Selection of displayed model
            if (ImGui::Combo("Filter", &filterIndex, filterNames.data(), filterNames.size())) {
                std::cout << filters[filterIndex] << std::endl;
                gridRenderer.initialize(filters[filterIndex].c_str());
            }
        }
        ImGui::End();
    }

    sgl::ImGuiWrapper::get()->renderEnd();
}

void MainApp::update(float dt) {
    AppLogic::update(dt);

    if (sgl::Keyboard->keyPressed(SDLK_UP)) {
        filterIndex = (filterIndex+1) % filters.size();
        std::cout << filters[filterIndex] << std::endl;
        gridRenderer.initialize(filters[filterIndex].c_str());
    }
    if (sgl::Keyboard->keyPressed(SDLK_DOWN)) {
        filterIndex = (filterIndex-1 + filters.size()) % filters.size();
        std::cout << filters[filterIndex] << std::endl;
        gridRenderer.initialize(filters[filterIndex].c_str());
    }

}
