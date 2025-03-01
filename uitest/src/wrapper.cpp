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
    // Initialize GLFW and OpenGL (assuming this is your backend)
    // if (!glfwInit())
        // return;

    // Create a windowed mode window and its OpenGL context
    // g_Window = glfwCreateWindow(1280, 720, "Dear ImGui Example", NULL, NULL);
    // if (!g_Window) {
        // glfwTerminate();
        // return;
    // }
    // glfwMakeContextCurrent(g_Window);
    // glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader (this example uses glad, you might need to use glew or other loaders)
    // gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

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

    // glfwDestroyWindow(g_Window);
    // glfwTerminate();
}

void ImGui_NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGui_Render() {
    // Render Dear ImGui into the current OpenGL context
    ImGui::Render();
    // int display_w, display_h;
    // glfwGetFramebufferSize(glfwGetCurrentContext(), &display_w, &display_h);
    // glViewport(0, 0, display_w, display_h);
    // glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    // glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // glfwSwapBuffers(g_Window);
}

void ImGui_Begin(const char *text) {
  ImGui::Begin(text);
}

void ImGui_End() {
  ImGui::End();
}

void camera_params(float *const rotation_speed, float *mov_speed, float *fov, float *mouse_sensitivity) {
    ImGui::Text("This is some useful text.");
    ImGui::SliderFloat("rad/s", rotation_speed, 0.5f, 10.0f);
    ImGui::SliderFloat("speed", mov_speed, 0.5f, 25.0f);
    ImGui::Text("Field of View:");
    ImGui::SliderFloat("##fov ", fov, 30.0f, 120.0f);
    ImGui::Text("Mouse Sensitivy");
    ImGui::SliderFloat("##msense", mouse_sensitivity, 1.0f, 15.0f);
}

void scene_params(float *lightCol, float *objectCol) {
  ImGui::SeparatorText("Scene Params");
  ImGui::ColorPicker3("Light Color", lightCol);
  ImGui::ColorPicker3("Object Color", objectCol);
}

void material_params(int *shiny, float *ambientStregth, float *diffuseStrength, float *specularStrength)  {
  ImGui::Text("Shinyness:");
  ImGui::SliderInt("##Shinyness", shiny, 2, 512);

  if (ImGui::TreeNode("Material Properties")) {
    ImGui::SliderFloat("ambient", ambientStregth, 0.0f, 1.0f);
    ImGui::SliderFloat("diffuse", diffuseStrength, 0.0f, 1.0f);
    ImGui::SliderFloat("specular", specularStrength, 0.0f, 1.0f);
    ImGui::TreePop();
  }
}

void demo() {
  ImGui::ShowDemoWindow();
}

} // extern "C"
