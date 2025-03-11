#ifndef WRAPPER
#define WRAPPER

#ifdef __cplusplus
extern "C" {
#endif

// typedef struct GLFWwindow GLFWwindow;

void ImGui_Init();
void ImGui_Deinit();
void ImGui_Begin(const char *text);
void ImGui_End();
void ImGui_NewFrame();
void ImGui_Render();
void camera_params(float *mov_speed, float *fov, float *mouse_sensitivity);
// void scene_params(float *lightCol, float *objectCol);
// void material_params(int *shiny, float *ambientStregth, float *diffuseStrength, float *specularStrength);
void demo();

#ifdef __cplusplus
}
#endif

#endif /* WRAPPER */
