#include "gui/gui.h"

#include <GLFW/glfw3.h>

#include "core/logger.h"

Gui::Gui()
{
}

void Gui::initialize(const char* glslVersion, GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Adapt style
    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;
    style->WindowTitleAlign = ImVec2(0.5F, 0.5F);

    style->WindowMinSize = ImVec2(700, 300);
}

void Gui::prepare()
{
    m_numDrawcalls = numDrawcalls;
    numDrawcalls = 0;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::defineWindow()
{
    ImGui::Begin("Doomed ImGui");
    ImGui::Checkbox("wireframe", &(Gui::isWireframeModeEnabled));

    /*ImGui::TextColored(ImVec4(1, 1, 0, 1), "Ambient");
    ImGui::DragFloat("Red", &ambient[0], 0.1f);
    ImGui::DragFloat("Green", &ambient[1], 0.1f);
    ImGui::DragFloat("Blue", &ambient[2], 0.1f);

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "lightWorldPos");
    ImGui::DragFloat("X", &lightWorldPos[0], 1.0f);
    ImGui::DragFloat("Y", &lightWorldPos[1], 1.0f);
    ImGui::DragFloat("Z", &lightWorldPos[2], 1.0f);
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "lightIntensity");
    ImGui::DragFloat("R", &lightIntensity[0], 10.0f);
    ImGui::DragFloat("G", &lightIntensity[1], 10.0f);
    ImGui::DragFloat("B", &lightIntensity[2], 10.0f);*/

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Number of Drawcalls");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", m_numDrawcalls);

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Rendering performance");
    ImGui::TextColored(
        ImVec4(1, 1, 0, 1),
        "Time per frame %d ms (%d fps)",
        static_cast<int>(1000 * lastFrameTime),
        static_cast<int>(1.0f / lastFrameTime));

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Position");
    ImGui::TextColored(
        ImVec4(1, 1, 0, 1), "x: %.2f y: %.2f z: %.2f", worldPos.x, worldPos.y, worldPos.z);

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Camera");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "yaw: %.2f pitch: %.2f", cameraYaw, cameraPitch);
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Log");

    ImGui::BeginChild("Scrolling");
    ImGui::TextUnformatted(Logger::getUiLogStream()->str().c_str());
    ImGui::EndChild();
    ImGui::End();
}

void Gui::render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}