#include "gui/gui.h"
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
}

void Gui::prepare()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::defineWindow()
{
    ImGui::Begin("Doomed ImGui");
    ImGui::Checkbox("wireframe", &(Gui::isWireframeModeEnabled));

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Ambient");
    ImGui::InputFloat("Red", &ambient[0], 0);
    ImGui::InputFloat("Green", &ambient[1], 0);
    ImGui::InputFloat("Blue", &ambient[2], 0);


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