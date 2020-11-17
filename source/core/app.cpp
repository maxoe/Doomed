#include "core/app.h"

#include "renderer/app_shader.h"

#include "vendor/imgui_impl_glfw.h"
#include "vendor/imgui_impl_opengl3.h"

#include <cassert>
#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include "world/maze.h"

App* App::instance = nullptr;

App::App()
{
    assert(!App::instance);
    if (App::instance == nullptr)
    {
        App::instance = this;
    }

    this->gui = new Gui();
}

int App::initialize()
{
    // set before init to get init errors
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error" << error << ": " << description << std::endl;
    });

    const auto glfwError = initializeGLFW();
    if (glfwError == 0)
    {
        return glfwError;
    }

    const auto gladError = gladLoadGL();
    if (gladError == 0)
    {
        glfwTerminate();
        std::cerr << "Error while loading Open GL" << std::endl;
        return 1;
    }

    gui->initialize(nullptr, window);

    glEnable(GL_DEPTH_TEST);

    return 0;
}

int App::initializeGLFW()
{
    if (glfwInit() == GLFW_FALSE)
    {
        return 0;
    }

#if __APPLE__
    glslVersion = "#version 150";                   // does not match gl version below 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 3.2 because
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);  // mac
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 3.3 because
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // not everyone can do 4.x
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // macOS

    glfwWindowHint(GLFW_SAMPLES, 8);  // 8x antialiasing

    // for debugging adjust to your display resolution
    const auto width = 3840;
    const auto height = 2160;
    window =
        glfwCreateWindow(width, height, "Doomed", nullptr /*glfwGetPrimaryMonitor()*/, nullptr);

    if (window == nullptr)
    {
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(
        window, [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); });

    // TODO: Register event callback

    return 1;
}

int App::mainLoop()
{
    // TODO TO BE CONTROLLED BY IMGUI
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Maze maze;
    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians((float)140), glm::vec3(0.0f, 1.0f, 0.0f));

    maze.addNode()->addModel("debug/house.obj", modelMatrix);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        gui->prepare();
        gui->defineWindow();

        // render
        auto width = 0;
        auto height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        maze.draw();

        gui->render();

        glfwSwapBuffers(window);
    }

    gui->shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}