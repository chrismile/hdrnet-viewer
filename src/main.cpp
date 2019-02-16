//============================================================================
// Name        : stencil-shadows-2d.cpp
// Author      : Christoph Neuhauser
// Copyright   : BSD 3-Clause License (for more details see LICENSE file)
//============================================================================

#include <iostream>
#include <Utils/File/FileUtils.hpp>
#include <Utils/AppSettings.hpp>
#include <Graphics/Window.hpp>

#include "MainApp.hpp"

using namespace std;
using namespace sgl;

int main(int argc, char *argv[]) {
    FileUtils::get()->initialize("hdrnet-viewer", argc, argv);

    // Load the file containing the app settings
    string settingsFile = FileUtils::get()->getConfigDirectory() + "settings.txt";
    AppSettings::get()->loadSettings(settingsFile.c_str());

    Window *window = AppSettings::get()->createWindow();
    AppSettings::get()->initializeSubsystems();

    AppLogic *app = new VolumeLightApp();
    app->run();

    delete app;
    AppSettings::get()->release();
    delete window;

    return 0;
}
