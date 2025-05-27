#include "GuiManager.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void GuiManager::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void GuiManager::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiManager::RenderStartScreen(bool& showStartScreen, bool& fadeOut, float& fadeOpacity) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 screenSize = io.DisplaySize;

    // Dibujar color de fondo con opacidad
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    ImVec4 backgroundColor = ImVec4(0.1f, 0.2f, 0.4f, fadeOpacity);  // Color personalizado
    draw_list->AddRectFilled(ImVec2(0, 0), screenSize, ImGui::GetColorU32(backgroundColor));

    // Centrar botón "Start"
    ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2, screenSize.y / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("StartScreen", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Start", ImVec2(100, 50))) {
        fadeOut = true;
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
