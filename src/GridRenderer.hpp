/*
 * GridRenderer.hpp
 *
 *  Created on: 05.10.2017
 *      Author: Christoph Neuhauser
 */

#ifndef GRIDRENDERER_HPP_
#define GRIDRENDERER_HPP_

#include <vector>
#include <Math/Geometry/AABB2.hpp>
#include <Graphics/Shader/ShaderManager.hpp>
#include <Graphics/Texture/TextureManager.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/Mesh/Vertex.hpp>
#include "GridPredictor.hpp"
#include "FrameData.hpp"

//! Used for rendering an image with a filter applied
class GridRenderer
{
public:
	GridRenderer();
	//! \param path: Path to folder containing effect data
	void initialize(const char *path);
	//! Renders imageTexture with filter applied. Transform coefficients are predicted using lowresImage.
	void renderTransformedImage(sgl::TexturePtr &imageTexture, FrameDataPtr &lowresImage);
	//! Renders imageTexture normally (no filter applied).
	void renderNormalImage(sgl::TexturePtr &imageTexture, FrameDataPtr &lowresImage);

private:
	void loadGuideParameters(const char *path);
	std::vector<sgl::VertexTextured> createTexturedQuad(const sgl::AABB2 &renderRect);

	GridPredictor gridPredictor;
	std::vector<sgl::TexturePtr> gridTextures;

	sgl::ShaderProgramPtr gridRenderShader;
	sgl::ShaderProgramPtr blitShader;
};

#endif /* GRIDRENDERER_HPP_ */
