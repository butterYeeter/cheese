#include <stdio.h>
#include <GLFW/glfw3.h>
#include "wrapper.h"

int main() {
  if (!glfwInit()) {
    printf("Glfw init failed!\n");
    return -1;
  }

  GLFWwindow *win = glfwCreateWindow(800, 800, "HELLO IMGUI", NULL, NULL);
  if (!win) {
    printf("Failed to create window!\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);


  ImGui_Init();

  while (!glfwWindowShouldClose(win)) {
    ImGui_NewFrame();

    ImGui_BuildUI();

    ImGui_Render();
    glfwSwapBuffers(win);

    glfwPollEvents();

  }

  ImGui_Deinit();
  glfwTerminate();
  return 0;
}