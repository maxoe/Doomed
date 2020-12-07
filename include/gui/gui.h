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
    glm::vec3 ambient;
    glm::vec3 lightWorldPos;
    glm::vec3 lightIntensity;

    int numDrawcalls = 0;
    float lastFrameStart = 0;
    float lastFrameTime = 0;

private:
    int m_numDrawcalls = 0;
};
