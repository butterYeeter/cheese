#include "imgui.h"
#include "wrapper.h"

// Platform and Renderer bindings (this is just an example; you'll need to include your actual platform/renderer bindings)
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

// Global context (You can manage this more carefully if you need thread safety, etc.)
// static GLFWwindow* g_Window = nullptr;

extern "C" {

void ImGui_Init() {
    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void ImGui_Deinit() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGui_NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGui_Render() {
    // Render Dear ImGui into the current OpenGL context
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_Begin(const char *text) {
  ImGui::Begin(text);
}

void ImGui_End() {
  ImGui::End();
}

void camera_params(float *const rotation_speed, float *mov_speed, float *fov, float *mouse_sensitivity) {
  if (ImGui::TreeNode("Camera Params")) {
    ImGui::SliderFloat("rad/s", rotation_speed, 0.5f, 10.0f);
    ImGui::SliderFloat("speed", mov_speed, 0.5f, 25.0f);
    ImGui::Text("Field of View:");
    ImGui::SliderFloat("##fov ", fov, 30.0f, 120.0f);
    ImGui::Text("Mouse Sensitivy");
    ImGui::SliderFloat("##msense", mouse_sensitivity, 1.0f, 15.0f);
    ImGui::TreePop();
  }
}


void demo() {
  ImGui::ShowDemoWindow();
}

} // extern "C"
