#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class App
{
public:
    App();

    int initialize();
    static App* getInstance();
    int mainLoop();

private:
    static App* instance;
    GLFWwindow* window;

    int initializeGLFW();
};