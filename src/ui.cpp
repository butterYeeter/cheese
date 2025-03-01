#include <stdint.h>
#include "ui.h"
#include "imgui.h"
#include <string>

using namespace std;

void modify_scene_params(PointLight lights[], uint32_t num_lights) {
  if (ImGui::TreeNode("Light Parameters")) {
    for (int i = 0; i < num_lights; i++) {
      std::string title = "Light #" + to_string(i);

      if (ImGui::TreeNode(title.c_str())) {
        ImGui::DragFloat3("##Position", lights[i].position, 0.1f);
        ImGui::ColorEdit3("##Ambient", lights[i].ambient);
        ImGui::ColorEdit3("##Diffuse", lights[i].diffuse);
        ImGui::ColorEdit3("##Specular", lights[i].specular);

        ImGui::TreePop();
      }
    }
    ImGui::TreePop();
  }
}