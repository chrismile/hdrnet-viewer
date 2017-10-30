/*
 * GridRenderer.cpp
 *
 *  Created on: 05.10.2017
 *      Author: Christoph Neuhauser
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

void loadBytesFromFile(const std::string filename, int bytes, char *buffer) {
	std::ifstream file(filename.c_str(), std::ios::binary);
	if(!file.is_open()) {
		Logfile::get()->writeError(std::string() + "ERROR in loadFileContent: "
				+ "Couldn't load file " + filename);
	}
	file.read(buffer, bytes);
	file.close();
}

GridRenderer::GridRenderer() {
	gridRenderShader = ShaderManager->getShaderProgram({"ApplyCoefficients.Vertex", "ApplyCoefficients.Fragment"});
	blitShader = ShaderManager->getShaderProgram({"Blit.Vertex", "Blit.Fragment"});
}

void GridRenderer::initialize(const char *path) {
	gridPredictor = GridPredictor();
	gridPredictor.loadGraph(path);
	glm::ivec3 gridSize = gridPredictor.getGridSize();

	TextureSettings settings;
	settings.internalFormat = GL_RGBA16F;
	gridTextures.clear();
	for (int i = 0; i < 3; ++i) {
		TexturePtr gridTexture = TextureManager->createEmptyTexture3D(gridSize.x, gridSize.y, gridSize.z, settings);
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
	GeometryBufferPtr geomBuffer = Renderer->createGeometryBuffer(sizeof(VertexTextured)*fullscreenQuad.size(), &fullscreenQuad.front());
	sgl::ShaderAttributesPtr gridRenderData = ShaderManager->createShaderAttributes(gridRenderShader);
	gridRenderData->addGeometryBuffer(geomBuffer, "position", ATTRIB_FLOAT, 3, 0, stride);
	gridRenderData->addGeometryBuffer(geomBuffer, "texcoord", ATTRIB_FLOAT, 2, sizeof(glm::vec3), stride);


	float *affineCoefficients = gridPredictor.computeGridCoefficients(lowresImage);
	glm::ivec3 gridSize = gridPredictor.getGridSize();

	gridRenderShader->setUniform("image", imageTexture, 0);
	for (int i = 0; i < 3; ++i) {
		float *data = affineCoefficients + i*gridSize.x*gridSize.y*gridSize.z*4;
		TexturePtr gridTexture = gridTextures[i];
		gridTexture->uploadPixelData3D(gridSize.x, gridSize.y, gridSize.z, data, PixelFormat(GL_RGBA, GL_FLOAT));
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
	GeometryBufferPtr geomBuffer = Renderer->createGeometryBuffer(sizeof(VertexTextured)*fullscreenQuad.size(), &fullscreenQuad.front());
	sgl::ShaderAttributesPtr renderData = ShaderManager->createShaderAttributes(blitShader);
	renderData->addGeometryBuffer(geomBuffer, "position", ATTRIB_FLOAT, 3, 0, stride);
	renderData->addGeometryBuffer(geomBuffer, "texcoord", ATTRIB_FLOAT, 2, sizeof(glm::vec3), stride);
	blitShader->setUniform("texture", imageTexture);

	Renderer->render(renderData);
}

// See https://github.com/mgharbi/hdrnet/blob/master/benchmark/src/renderer.cc for more details
void GridRenderer::loadGuideParameters(const char *path) {
	glm::mat3x4 ccm;
	glm::vec4 mixMatrix;
	glm::vec3 shifts[16];
	glm::vec3 slopes[16];

	loadBytesFromFile(std::string() + path + "guide_ccm_f32_3x4.bin", sizeof(glm::mat3x4), (char*)&ccm);
	loadBytesFromFile(std::string() + path + "guide_mix_matrix_f32_1x4.bin", sizeof(glm::vec4), (char*)&mixMatrix);
	loadBytesFromFile(std::string() + path + "guide_shifts_f32_16x3.bin", 16*3*sizeof(float), (char*)shifts);
	loadBytesFromFile(std::string() + path + "guide_slopes_f32_16x3.bin", 16*3*sizeof(float), (char*)slopes);

	gridRenderShader->setUniform("guideCCM", ccm);
	gridRenderShader->setUniform("mixMatrix", mixMatrix);
	gridRenderShader->setUniformArray("guideShifts", shifts, 16);
	gridRenderShader->setUniformArray("guideSlopes", slopes, 16);
}


std::vector<VertexTextured> GridRenderer::createTexturedQuad(const AABB2 &renderRect)
{
	glm::vec2 min = renderRect.getMinimum();
	glm::vec2 max = renderRect.getMaximum();
	std::vector<VertexTextured> quad{
		VertexTextured(glm::vec3(max.x,max.y,0), glm::vec2(1, 0)),
		VertexTextured(glm::vec3(min.x,min.y,0), glm::vec2(0, 1)),
		VertexTextured(glm::vec3(max.x,min.y,0), glm::vec2(1, 1)),
		VertexTextured(glm::vec3(min.x,min.y,0), glm::vec2(0, 1)),
		VertexTextured(glm::vec3(max.x,max.y,0), glm::vec2(1, 0)),
		VertexTextured(glm::vec3(min.x,max.y,0), glm::vec2(0, 0))};
	return quad;
}
