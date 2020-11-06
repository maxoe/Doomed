#include "core/app.h"

#include "renderer/app_shader.h"

#include <cassert>
#include <iostream>

App* App::instance = nullptr;

App::App()
{
    assert(!App::instance);
    if (App::instance == nullptr)
    {
        App::instance = this;
    }
}

int App::initialize()
{
    // set before init to get init errors
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error" << error << ": " << description << std::endl;
    });

    const auto glfwError = initializeGLFW();
    if (glfwError != 0)
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

    return 0;
}

int App::initializeGLFW()
{
    if (glfwInit() == GLFW_FALSE)
    {
        return 1;
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
    const auto height = 1860;
    window = glfwCreateWindow(width, height, "Fancy Renderstuff", glfwGetPrimaryMonitor(), nullptr);

    if (window == nullptr)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowSizeCallback(
        window, [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); });

    // TODO: Register event callback

    return 0;
}

int App::mainLoop()
{
    // SOME EXAMPLE GEOMETRY
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,
        -1.0f,
        0.0f,
        1.0f,
        -1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // FINISHED SETTING UP EXAMPLE GEOMETRY

    AppShader program("simple");

    glUseProgram(program.getProgramId());

    while (!glfwWindowShouldClose(window))
    {
        auto width = 0;
        auto height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DRAW EXAMPLE GEOMETRY
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,  // size
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (void*)0   // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3);  // 3 indices starting at 0 -> 1 triangle
        glDisableVertexAttribArray(0);

        // FINISHED DRAWING EXAMPLE GEOMTRY
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}