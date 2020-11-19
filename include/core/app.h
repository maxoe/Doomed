#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "gui/gui.h"

class App
{
public:
    App();
    ~App()
    {
        delete gui;
    }

    int initialize();
    void shutdown();
    static App* getInstance();
    int mainLoop();

private:
    static App* instance;
    GLFWwindow* window;
    Gui* gui;

    int initializeGLFW();
    int initializeImgui();
};