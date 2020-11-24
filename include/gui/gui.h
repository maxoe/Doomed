#include <imgui.h>
#include "vendor/imgui_impl_glfw.h"
#include "vendor/imgui_impl_opengl3.h"
#include <sstream>
#include <glm/glm.hpp>

class Gui
{
public:
    Gui();

    void initialize(const char* glslVersion, GLFWwindow* window);
    void shutdown();
    void prepare();
    void defineWindow();
    void render();

    bool isWireframeModeEnabled = false;
    glm::vec3 ambient = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 lightWorldPos = glm::vec3(0.0f, 3.0f, 10.0f);
    glm::vec3 lightIntensity = glm::vec3(0.0f, 940.0f, 620.0f);

private:
};
