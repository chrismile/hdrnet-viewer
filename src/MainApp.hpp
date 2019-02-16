/*
 * MainApp.hpp
 *
 *  Created on: 04.10.2017
 *      Author: Christoph Neuhauser
 */

#ifndef LOGIC_VolumeLightApp_HPP_
#define LOGIC_VolumeLightApp_HPP_

#include <Utils/AppLogic.hpp>
#include <Graphics/Shader/ShaderAttributes.hpp>
#include <Math/Geometry/Point2.hpp>
#include <Graphics/Scene/Camera.hpp>
#include <vector>
#include <glm/glm.hpp>
#include "GridRenderer.hpp"
#include "Webcam.hpp"

using namespace std;
using namespace sgl;

class VolumeLightApp : public AppLogic
{
public:
    VolumeLightApp();
    ~VolumeLightApp();
    void render();
    void update(float dt);
    void resolutionChanged(EventPtr event);

private:
    Webcam webcam;
    FrameDataPtr frameImage;
    FrameDataPtr downscaledImage;
    TexturePtr frameTexture;
    TexturePtr downscaledTexture;

    // Lighting & rendering
    GridRenderer gridRenderer;

    // User interaction
    std::vector<std::string> filters;
    int filterIndex;
};

#endif /* LOGIC_VolumeLightApp_HPP_ */
