#include "core/app.h"

#include <cassert>

#include <glm/gtx/string_cast.hpp>

#include "world/maze.h"
#include "core/logger.h"

App* App::instance = nullptr;

App::App()
{
    assert(!instance);
    if (instance == nullptr)
    {
        instance = this;
    }

    this->gui = new Gui();

    LOG_CORE_INFO("GUI has been successfully initialized");
}

int App::initialize()
{
    // set before init to get init errors
    glfwSetErrorCallback([](int error, const char* description) {
        LOG_CORE_ERROR("GLFW Error: " + std::string(description));
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
        LOG_CORE_ERROR("Error while loading Open GL");
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
    Maze maze;
    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
    modelMatrix = glm::rotate(
        modelMatrix, glm::radians(static_cast<float>(180)), glm::vec3(0.0f, 1.0f, 0.0f));

    maze.addNode()->addModel("node1/blender.obj", modelMatrix);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        gui->prepare();
        gui->defineWindow();

        // TODO one ui for game one for rendering debug
        if (gui->isWireframeModeEnabled)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        // render
        auto width = 0;
        auto height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        maze.draw(gui->ambient);

        gui->render();

        glfwSwapBuffers(window);
    }

    shutdown();

    return 0;
}

void App::shutdown()
{
    gui->shutdown();
    LOG_CLEANUP();
    glfwDestroyWindow(window);
    glfwTerminate();
}
