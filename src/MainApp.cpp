/*
 * MainApp.cpp
 *
 *  Created on: 04.10.2017
 *      Author: Christoph Neuhauser
 */

#include "MainApp.hpp"

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

VolumeLightApp::VolumeLightApp()
{
	EventManager::get()->addListener(RESOLUTION_CHANGED_EVENT, [this](EventPtr event){ this->resolutionChanged(event); });

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
			//"Data/pretrained_models/local_laplacian/normal_1024/",
			"Data/pretrained_models/local_laplacian/strong_1024/",
			//"Data/pretrained_models/photoshop/early_bird/",
			//"Data/pretrained_models/photoshop/false_colors/",
			//"Data/pretrained_models/photoshop/infrared/",
			//"Data/pretrained_models/photoshop/lomo_fi/",
	};
	filterIndex = 0;

	// Filter
	gridRenderer.initialize(filters[filterIndex].c_str());
}

VolumeLightApp::~VolumeLightApp()
{
}

void VolumeLightApp::render()
{
	Window *window = AppSettings::get()->getMainWindow();
	glViewport(0, 0, window->getWidth(), window->getHeight());

	if (webcam.readFrame(frameImage, downscaledImage)) {
		if (!frameTexture) {
			frameTexture = TextureManager->createEmptyTexture(frameImage->w, frameImage->h);
			downscaledTexture = TextureManager->createEmptyTexture(downscaledImage->w, downscaledImage->h);
		}
		frameTexture->uploadPixelData(frameImage->w, frameImage->h, frameImage->pixels);
		downscaledTexture->uploadPixelData(downscaledImage->w, downscaledImage->h, downscaledImage->pixels);
	}

	glm::mat4 newProjMat(matrixOrthogonalProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f));
	Renderer->setProjectionMatrix(newProjMat);
	Renderer->setViewMatrix(matrixIdentity());
	Renderer->setModelMatrix(matrixIdentity());

	Renderer->clearFramebuffer(GL_COLOR_BUFFER_BIT, Color(0, 0, 0));

	if (frameTexture) {
		if (Keyboard->isKeyDown(SDLK_SPACE)) {
			gridRenderer.renderNormalImage(frameTexture, downscaledImage);
		} else {
			gridRenderer.renderTransformedImage(frameTexture, downscaledImage);
		}

		Renderer->errorCheck();
	}
}

void VolumeLightApp::resolutionChanged(EventPtr event)
{
	Window *window = AppSettings::get()->getMainWindow();
	glViewport(0, 0, window->getWidth(), window->getHeight());
}

void VolumeLightApp::update(float dt)
{
	AppLogic::update(dt);

	if (Keyboard->keyPressed(SDLK_UP)) {
		filterIndex = (filterIndex+1) % filters.size();
		cout << filters[filterIndex] << endl;
		gridRenderer.initialize(filters[filterIndex].c_str());
	}
	if (Keyboard->keyPressed(SDLK_DOWN)) {
		filterIndex = (filterIndex-1 + filters.size()) % filters.size();
		cout << filters[filterIndex] << endl;
		gridRenderer.initialize(filters[filterIndex].c_str());
	}

}
