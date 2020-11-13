#include "core/app.h"

#include "renderer/app_shader.h"

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
    const auto height = 2160;
    window = glfwCreateWindow(width, height, "Doomed", glfwGetPrimaryMonitor(), nullptr);

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

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // cube and normals
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
        -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //// normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //// light cube
    // GLuint lightVao;
    // glGenVertexArrays(1, &lightVao);
    // glBindVertexArray(lightVao);
    //// vbo already bound
    //// set the vertex attribute
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    // FINISHED SETTING UP EXAMPLE GEOMETRY AND TRANSFORMATIONS

    AppShader program("simple");
    // AppShader lightProgram("simple");

    glEnable(GL_DEPTH_TEST);

    glm::vec3 camWorldPos(0.0f, 0.0f, 3.0);
    glm::vec3 cameraDir(1.2f, 1.0f, 2.0f);
    glm::vec3 lightWorldPos(1.2f, 1.0f, 2.0f);
    glm::vec3 lightIntensity(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        auto width = 0;
        auto height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw cube
        program.use();

        // model viel be in scene model, view-projection in camera
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(
            model, glm::radians((float)glfwGetTime() * 30), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(
            model, glm::radians((float)glfwGetTime() * 45), glm::vec3(0.0f, 1.0f, 0.0f));

        // breaks lightning for some reason it's very dark
        glm::mat4 view =
            glm::lookAt(camWorldPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection =
            glm::perspective(glm::radians(45.0f), 3840.0f / 2160.0f, 0.1f, 100.0f);

        program.setMat4f("modelMatrix", model);
        program.setMat4f("MVP", projection * view * model);

        glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(model));

        program.setMat3f("normalMatrix", normalMatrix);

        program.setVec3f("kD", 1.0f, 1.0f, 5.0f);
        program.setVec3f("kS", 0.0f, 0.0f, 3.0f);
        program.setFloat("n", 16.0f);
        program.setVec3f("lightWorldPos", lightWorldPos);
        program.setVec3f("lightIntensity", lightIntensity);
        program.setVec3f("camWorldPos", camWorldPos);

        // DRAW EXAMPLE GEOMETRY
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //// draw light cube
        // lightProgram.use();

        // model = glm::mat4(1.0f);
        // model = glm::translate(model, lightWorldPos);
        // model = glm::scale(model, glm::vec3(0.2f));
        // normalMatrix = glm::inverse(glm::transpose(glm::mat3(model)));

        // program.setMat4f("view", view);
        // program.setMat4f("projection", projection);

        // program.setVec3f("objectColor", lightIntensity);
        // program.setVec3f("lightIntensity", lightIntensity);
        // program.setVec3f("lightWorldPos", lightWorldPos);
        // program.setMat4f("normalMatrix", normalMatrix);
        // program.setVec3f("viewPos", camWorldPos);

        // std::cout << glm::to_string(lightWorldPos) << std::endl;
        // std::cout << glm::to_string(camWorldPos) << std::endl;

        // glBindVertexArray(lightVao);
        // glDrawArrays(GL_TRIANGLES, 0, 108);
        // glDisableVertexAttribArray(vao);

        // FINISHED DRAWING EXAMPLE GEOMTRY
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}