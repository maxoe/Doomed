#include <imgui.h>
#include "vendor/imgui_impl_glfw.h"
#include "vendor/imgui_impl_opengl3.h"

class Gui
{
public:
    Gui();

    void initialize(const char* glslVersion, GLFWwindow* window);
    void shutdown();
    void prepare();
    void defineWindow();
    void render();

private:
};
