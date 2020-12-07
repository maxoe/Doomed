#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "gui/gui.h"
#include "core/input_dispatcher.h"
#include "world/maze.h"

class App
{
public:
    App();

    int initialize();
    void shutdown();
    static App* getInstance();

    int mainLoop();
    void processInput();

    GLFWwindow* getWindow();
    Gui& getGui();
    InputDispatcher& getInputDispatcher();
    Maze& getMaze();

    void setWidth(float width);
    void setHeight(float height);
    [[nodiscard]] float getWidth() const;
    [[nodiscard]] float getHeight() const;

private:
    static App* instance;
    GLFWwindow* window;
    float width;
    float height;
    Gui gui;
    InputDispatcher inputDispatcher;
    Maze maze;

    int initializeGLFW();
    int initializeImgui();
};