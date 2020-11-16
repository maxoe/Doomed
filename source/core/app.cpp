#include "core/app.h"

#include "renderer/app_shader.h"
#include "world/maze_node.h"

#include "vendor/imgui_impl_glfw.h"
#include "vendor/imgui_impl_opengl3.h"

#include <cassert>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

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

    gui->initialize(NULL, window);

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
    AppShader program("simple");

    glm::vec3 camWorldPos(0.0f, 0.8f, 3.0);
    glm::vec3 cameraDir(1.2f, 1.0f, 2.0f);
    glm::vec3 lightWorldPos(5.0f, 5.0f, 2.0f);
    glm::vec3 lightIntensity(50.0f, 40.0f, 40.0f);

    MazeNode firstNode;
    firstNode.addObject("debug/house.obj");

    // TODO MOVE TO BE CONTROLLED BY IMGUI
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // setup program and transformations will be moved to world class
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        model = glm::rotate(
            model, glm::radians((float)glfwGetTime() * 30), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 view =
            glm::lookAt(camWorldPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, -7.0f, -25.0f));

        glm::mat4 projection =
            glm::perspective(glm::radians(45.0f), 3840.0f / 2160.0f, 0.1f, 100.0f);

        program.setMat4f("modelMatrix", model);
        program.setMat4f("MVP", projection * view * model);

        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));

        program.setMat3f("normalMatrix", normalMatrix);

        program.setVec3f("kD", 1.0f, 1.0f, 1.0f);
        program.setVec3f("kS", 0.0f, 0.0f, 0.0f);
        program.setFloat("n", 16.0f);
        program.setVec3f("lightWorldPos", lightWorldPos);
        program.setVec3f("lightIntensity", lightIntensity);
        program.setVec3f("camWorldPos", camWorldPos);

        gui->prepare();
        gui->defineWindow();

        // render
        auto width = 0;
        auto height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        firstNode.draw(program);

        gui->render();

        glfwSwapBuffers(window);
    }

    gui->shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}