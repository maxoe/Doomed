#include "core/app.h"

#include <cassert>

#include <glm/gtx/string_cast.hpp>

#include "world/maze.h"
#include "world/maze_node.h"
#include "core/logger.h"

#define PRINT_GL_CALLBACKS 0

App* App::instance = nullptr;

App::App()
    : width(3840)  // initial width and height
    , height(2160)
{
    assert(!instance);
    if (instance == nullptr)
    {
        instance = this;
    }

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

    gui.initialize(nullptr, window);
    maze = Maze("deferred");
    inputDispatcher = InputDispatcher(width, height);

#if PRINT_GL_CALLBACKS
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        [](GLenum source,
           GLenum type,
           GLuint id,
           GLenum severity,
           GLsizei length,
           const GLchar* message,
           const void* userParam) {
            fprintf(
                stderr,
                "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type,
                severity,
                message);
        },
        nullptr);
#endif

    return 0;
}

int App::initializeGLFW()
{
    if (glfwInit() == GLFW_FALSE)
    {
        return 0;
    }

#if __APPLE__
    glslVersion = "#version 150";  // does not match gl version below 3.3
    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3);  // 3.2 because 4.3 only for debugging and don't know if works well on osx
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);  // and mac does not like 3.3
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // need 4.3 because error callbacks
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // macOS

    glfwWindowHint(GLFW_SAMPLES, 8);  // 8x antialiasing

    window =
        glfwCreateWindow(width, height, "Doomed", nullptr /*glfwGetPrimaryMonitor()*/, nullptr);

    if (window == nullptr)
    {
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        getInstance()->setWidth(width);
        getInstance()->setHeight(height);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
        getInstance()->inputDispatcher.processMouseMovement(
            getInstance()->getMaze().getCamera(), xPos, yPos);
    });

    glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int entered) {
        if (!entered)
        {
            getInstance()->getInputDispatcher().onMouseOut();
        }
    });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return 1;
}

int App::mainLoop()
{
    // glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.005f, 0.005f, 0.005f));
    // modelMatrix = glm::rotate(
    // modelMatrix, glm::radians(static_cast<float>(180)), glm::vec3(0.0f, 1.0f, 0.0f));

    maze.addNode()
        ->addModel("debug/sponza/sponza.obj", 50.0f)
        //->attachModelToLast("sci-fi/hallway.obj", AttachmentPoint::PosZ, false)
        ->addPointLight(glm::vec3(15.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 20)
        ->addPointLight(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(2.0f), 50)
        ->setDirectionalLight(glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3(0.3f));

    while (!glfwWindowShouldClose(window))
    {
        gui.lastFrameTime = glfwGetTime() - gui.lastFrameStart;
        gui.lastFrameStart = glfwGetTime();

        glfwPollEvents();

        processInput();

        gui.prepare();
        gui.defineWindow();

        // TODO one ui for game one for rendering debug
        if (gui.isWireframeModeEnabled)
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        maze.draw(/*gui->ambient*/);

        gui.render();

        glfwSwapBuffers(window);
    }

    shutdown();

    return 0;
}

void App::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    InputDispatcher::Movement keys = InputDispatcher::Movement::NONE;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        keys |= InputDispatcher::Movement::FRONT;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        keys |= InputDispatcher::Movement::BACK;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        keys |= InputDispatcher::Movement::LEFT;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        keys |= InputDispatcher::Movement::RIGHT;
    }

    inputDispatcher.processKeyInput(keys, maze.getCamera());
}

void App::shutdown()
{
    gui.shutdown();
    LOG_CLEANUP();
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* App::getWindow()
{
    return window;
}

Gui& App::getGui()
{
    return gui;
}

InputDispatcher& App::getInputDispatcher()
{
    return inputDispatcher;
}

Maze& App::getMaze()
{
    return maze;
}

App* App::getInstance()
{
    return instance;
}

void App::setWidth(float width)
{
    this->width = width;
}
void App::setHeight(float height)
{
    this->height = height;
}