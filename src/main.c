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
#include "ebo.h"
#include "kvec.h"
#include "mesh.h"
#include "model.h"
#include "shader.h"
#include "lights.h"
#include "shapes.h"
#include "texture.h"
#include "ui.h"
#include "vao.h"
#include "vbo.h"


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

  ShaderProgram default_shader = shaderprogram_create(shaders_dir, "default/vert.glsl", "default/frag.glsl");
  ShaderProgram fbshader = shaderprogram_create(shaders_dir, "framebuffer/vert.glsl", "framebuffer/base.frag.glsl");

  vec3 cam_pos = {0.0f, 0.0f, 3.0f};
  vec3 look_dir;
  Camera cam = camera_create(cam_pos, 10.0f);
  camera_get_look_dir(cam, look_dir);
  glm_vec3_negate(look_dir);
  Camera rear_mirror = camera_create(cam_pos, 10.0f);
  camera_set_look_dir(rear_mirror, look_dir);

  int mirror_width = 600;
  int mirror_height = 200;

  mat4 projmirror;
  glm_perspective(glm_rad(45.0f), (float)mirror_width/mirror_height, 0.1f, 100.0f, projmirror);

  float cur_time = glfwGetTime();
  float last_time = cur_time;
  double m_x, m_y;
  float last_m_x, last_m_y;
  glfwGetCursorPos(win, &m_x, &m_y);
  last_m_x = (float)m_x;
  last_m_y = (float)m_y;

  VAO screenvao = vao_create_new();
  VBO screenvbo = vbo_create_new(GL_ARRAY_BUFFER);

  vao_bind(screenvao);
  vbo_bind(screenvbo);
  vbo_buffer_data(screenvbo, sizeof_square, square, GL_STATIC_DRAW);

  vao_enable_index(screenvao, 0);
  vao_attrib_pointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
  vao_enable_index(screenvao, 1);
  vao_attrib_pointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

  Model plane, container;
  model_create(&plane, objects_dir, "plane/plane.obj");
  model_create(&container, objects_dir, "container/container.obj");

  ImGui_Init();

  Texture color = texture_create_from_mem(texture_none, false, mirror_width, mirror_height, 3, NULL);

  GLuint rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mirror_width, mirror_height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_get_id(color), 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("ERROR::Framebuffer:: Framebuffer is not complete!\n");
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  while (!glfwWindowShouldClose(win)) {
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

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, mirror_width, mirror_height);

    vec3 u = {0.0f, 1.0f, 0.0f};
    vec3 r, f, p;
    camera_get_pos(cam, p);
    camera_get_look_dir(cam, look_dir);
    glm_vec3_inv_to(look_dir, f);
    camera_set_pos(rear_mirror, p);
    camera_set_look_dir(rear_mirror, f);

    mat4 model, view, proj;
    camera_get_matrix(rear_mirror, view);
    glm_perspective(glm_rad(fov), (float)win_width/win_height, 0.1f, 100.0f, proj);

    shaderprogram_use(default_shader);
    shaderprogram_set_mat4(default_shader, "view", view[0]);
    shaderprogram_set_mat4(default_shader, "proj", projmirror[0]);

    glm_mat4_identity(model);
    glm_translate_y(model, -5.0f);
    glm_scale_uni(model, 5.0f);
    shaderprogram_set_mat4(default_shader, "model", model[0]);
    model_draw(plane, default_shader);
     
    glm_mat4_identity(model);
    glm_translate(model, (vec3){0.0f, -3.5f, 2.0f});
    shaderprogram_set_mat4(default_shader, "model", model[0]);
    model_draw(container, default_shader);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, win_width, win_height);

    camera_get_matrix(cam, view);
    glm_perspective(glm_rad(fov), (float)win_width/win_height, 0.1f, 100.0f, proj);

    shaderprogram_use(default_shader);
    shaderprogram_set_mat4(default_shader, "view", view[0]);
    shaderprogram_set_mat4(default_shader, "proj", proj[0]);

    glm_mat4_identity(model);
    glm_translate_y(model, -5.0f);
    glm_scale_uni(model, 5.0f);
    shaderprogram_set_mat4(default_shader, "model", model[0]);
    model_draw(plane, default_shader);
     
    glm_mat4_identity(model);
    glm_translate(model, (vec3){0.0f, -3.5f, 2.0f});
    shaderprogram_set_mat4(default_shader, "model", model[0]);
    model_draw(container, default_shader);

    glDisable(GL_DEPTH_TEST);
    texture_bind(color, 0);
    shaderprogram_use(fbshader);
    vao_bind(screenvao);
    vbo_bind(screenvbo);
    glm_mat4_identity(model);
    glm_translate_y(model, 0.86f);
    glm_scale(model, (vec3){0.47f, 0.28f, 1.0f});
    shaderprogram_set_mat4(fbshader, "model", model[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

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

