#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>
#include <wrapper.h>


#include "camera.h"
#include "cglm/mat4.h"
#include "model.h"
#include "shader.h"
#include "lights.h"
#include "shapes.h"
#include "ui.h"


typedef struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
} Material;


int win_width = 1280, win_height = 720;

bool use_mouse = true;
bool firstPress = false;

float move_speed = 10.0f;
float fov = 45.0f;
float mouse_sensitivity = 10.0f;

PointLight lights[4];


void process_inputs(GLFWwindow * win) {
  if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS) {
    glfwSetWindowShouldClose(win, GLFW_TRUE);
  }

  bool currentlyPressed = glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS;
  if (!firstPress && currentlyPressed) {
    use_mouse = !use_mouse;
    if (use_mouse)
      glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
      glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  firstPress = currentlyPressed;
}

void window_resize(GLFWwindow *window, int width, int height) {
  win_width = width, win_height = height;
  glViewport(0, 0, win_width, win_height);
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *win = glfwCreateWindow(win_width, win_height, "OpenGL Rendering Project", NULL, NULL);
  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);
  glfwSetFramebufferSizeCallback(win, window_resize);
  glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    return 1;
  }
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  char shaders_dir[] = "resources/shaders/";
  char objects_dir[] = "resources/objects/";

  init_lights(lights, 4);
  // ShaderProgram program = shaderprogram_create(shaders_dir, "phong/vert.glsl", "phong/frag.glsl");
  // shaderprogram_use(program);
  // vec3 zero = {0.0f, 0.0f, 0.0f};
  // shaderprogram_set_float(program, "material.shininess", 128.0f);
  // shaderprogram_set_bool(program, "one_channel_specular", true);
  // shaderprogram_set_vec3(program, "dirLight.direction", 1.0f, -0.9f, -0.5f);
  // shaderprogram_set_vec3(program, "dirLight.ambient", 0.1*0.976, 0.1*0.990, 0.1*0.584);
  // shaderprogram_set_vec3(program, "dirLight.diffuse", 0.2*0.980, 0.2*0.990, 0.2*0.376);
  // shaderprogram_set_vec3(program, "dirLight.specular", 0.991, 1.00, 0.430);
  // update_lights(program, lights, 4);

  ShaderProgram depth_shader = shaderprogram_create(shaders_dir, "depth_test/vert.glsl", "depth_test/frag.glsl");

  Camera cam = camera_create((vec3){0.0f, 0.0f, 3.0f,}, 10.0f);
  float cur_time = glfwGetTime();
  float last_time = cur_time;
  double m_x, m_y;
  float last_m_x, last_m_y;
  glfwGetCursorPos(win, &m_x, &m_y);
  last_m_x = (float)m_x;
  last_m_y = (float)m_y;

  Model plane, cube;
  model_create(&plane, objects_dir, "plane/plane.obj");
  model_create(&cube, objects_dir, "cubey/cubey.obj");


  ImGui_Init();


  while (!glfwWindowShouldClose(win)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    ImGui_NewFrame();
    
    cur_time = glfwGetTime();
    float delta_time = cur_time - last_time;
    last_time = cur_time;
    process_inputs(win);
    camera_process_keys(cam, win, delta_time);

    float xoffset, yoffset;
    glfwGetCursorPos(win, &m_x, &m_y);
    xoffset = (float)m_x - last_m_x;
    yoffset = last_m_y - (float)m_y;
    last_m_x = (float)m_x;
    last_m_y = (float)m_y;

    if (use_mouse)
      camera_process_mouse_movement(cam, xoffset, yoffset, delta_time);


    mat4 view, proj;

    camera_get_matrix(cam, view);
    glm_perspective(glm_rad(fov), (float)win_width/win_height, 0.1f, 100.0f, proj);

    shaderprogram_use(depth_shader);
    shaderprogram_set_mat4(depth_shader, "view", view[0]);
    shaderprogram_set_mat4(depth_shader, "proj", proj[0]);

    mat4 model;
    glm_mat4_identity(model);
    shaderprogram_set_mat4(depth_shader, "model", model[0]);
    model_draw(plane, depth_shader);
     
    for (int i = 0; i < 1; i++) {
      mat4 model;
      glm_mat4_identity(model);
      glm_translate(model, (vec3){0.0f, -3.5f, i * 2.0f});
      shaderprogram_set_mat4(depth_shader, "model", model[0]);
      model_draw(cube, depth_shader);
    }

    // mat4 model, view, proj;
    // vec3 view_pos;

    // glm_mat4_identity(model);
    // camera_get_matrix(cam, view);
    // glm_perspective(glm_rad(fov), (float)win_width/win_height, 0.1f, 100.0f, proj);
    // camera_get_pos(cam, view_pos);
    
    // shaderprogram_use(program);
    // shaderprogram_set_mat4(program, "model", model[0]);
    // shaderprogram_set_mat4(program, "view", view[0]);
    // shaderprogram_set_mat4(program, "proj", proj[0]);
    // shaderprogram_set_vec3p(program, "viewPos", view_pos);
    // update_lights(program, lights, 4);
    // model_draw(plane, program);


    camera_set_speed(cam, move_speed);
    camera_set_mouse_sensitivity(cam, mouse_sensitivity);
    ImGui_Begin("Hello Window");
    camera_params(&move_speed, &fov, &mouse_sensitivity);
    uint32_t n = 4;
    modify_scene_params(lights, n);
    ImGui_End();
    // demo();
    ImGui_Render();

    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  ImGui_Deinit();
  glfwTerminate();
  return 0;
}

