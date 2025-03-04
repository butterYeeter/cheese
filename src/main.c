#include <math.h>
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
#include "shader.h"
#include "texture.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "lights.h"
#include "ui.h"
#include "mesh_loader.h"

#include "shapes.h"


typedef struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
} Material;


int win_width = 1280, win_height = 720;

bool use_mouse = true;
bool firstPress = false;

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

  VAO cubeVao;
  vao_create_new(&cubeVao);
  vao_bind(cubeVao);
  
  
  VBO vbo;
  vbo_create_new(&vbo, GL_ARRAY_BUFFER);
  vbo_bind(vbo);
  
  EBO ebo;
  ebo_create_new(&ebo, GL_ELEMENT_ARRAY_BUFFER);
  ebo_bind(ebo);
  
  vbo_buffer_data(vbo, sizeof_cube + sizeof_cube_tex_coords + sizeof_cube_normals, NULL, GL_STATIC_DRAW);
  vbo_buffer_subdata(0, sizeof_cube, cube);
  vbo_buffer_subdata(sizeof_cube, sizeof_cube_tex_coords, cube_tex_coords);
  vbo_buffer_subdata(sizeof_cube + sizeof_cube_tex_coords, sizeof_cube_normals, cube_normals);
  vao_enable_index(cubeVao, 0);
  vao_attrib_pointer(cubeVao, 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
  vao_enable_index(cubeVao, 1);
  vao_attrib_pointer(cubeVao, 1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)sizeof_cube);
  vao_enable_index(cubeVao, 2);
  vao_attrib_pointer(cubeVao, 2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float_t), (void*)(sizeof_cube+sizeof_cube_tex_coords));

  VAO lightVao;
  vao_create_new(&lightVao);
  vao_bind(lightVao);
  vao_enable_index(lightVao, 0);
  vao_attrib_pointer(lightVao, 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float_t), (void*)0);
  vao_bind(lightVao);
  
  init_lights(lights, 4);
  ShaderProgram program;
  shaderprogram_create(&program, "resources/shad.vert.glsl", "resources/woah.frag.glsl");
  shaderprogram_use(program);
  vec3 zero = {0.0f, 0.0f, 0.0f};
  shaderprogram_set_int(program, "material.diffuse", 0);
  shaderprogram_set_int(program, "material.specular", 1);
  shaderprogram_set_float(program, "material.shininess", 128.0f);
  shaderprogram_set_vec3(program, "dirLight.direction", 1.0f, -0.9f, -0.5f);
  shaderprogram_set_vec3(program, "dirLight.ambient", 0.2*0.976, 0.2*0.990, 0.2*0.584);
  shaderprogram_set_vec3(program, "dirLight.diffuse", 0.5*0.980, 0.5*0.990, 0.5*0.376);
  shaderprogram_set_vec3(program, "dirLight.specular", 0.991, 1.00, 0.430);
  update_lights(program, lights, 4);

  ShaderProgram lightShader;
  shaderprogram_create(&lightShader, "resources/light.vert.glsl", "resources/light.frag.glsl");


  vec3 lightPos = {1.2f, 1.0f, 2.0f};
  vec3 objectPos = {0.0f, -0.5f, 1.0f};

  Camera cam;
  camera_create(&cam, (vec3){0.0f, 0.0f, 3.0f,}, 10.0f);
  float cur_time = glfwGetTime();
  float last_time = cur_time;
  double m_x, m_y;
  float last_m_x, last_m_y;
  glfwGetCursorPos(win, &m_x, &m_y);
  last_m_x = (float)m_x;
  last_m_y = (float)m_y;


  Texture tex0, tex1, tex2;
  texture_create(&tex0, 0, "resources/container2.png");
  texture_create(&tex1, 1, "resources/container2_specular.png");

  ImGui_Init();

  float rotation_speed = 1.0f;
  float move_speed = 10.0f;
  float fov = 45.0f;
  float mouse_sensitivity = 10.0f;

  // {
    float *vbuf;
    uint32_t *ibuf;
    uint32_t vcount, icount;
    load_mesh_data(vbuf, &vcount, ibuf, &icount,
                   "model.obj");
    printf("Unique Vertices: %u, Number of Indices: %u", vcount, icount);
  // }

  while (!glfwWindowShouldClose(win)) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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


    vao_bind(lightVao);
    shaderprogram_use(lightShader);
    mat4 model, view, proj;
    camera_get_matrix(cam, view);
    glm_perspective(glm_rad(fov), (float)win_width/win_height, 0.1f, 100.0f, proj);
    for (int i = 0; i < 4; i++) {
      glm_mat4_identity(model);
      glm_translate(model, lights[i].position);
      glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});
      shaderprogram_set_mat4(lightShader, "model", model[0]);
      shaderprogram_set_mat4(lightShader, "view", view[0]);
      shaderprogram_set_mat4(lightShader, "proj", proj[0]);
      shaderprogram_set_vec3p(lightShader, "lightColor", lights[i].specular);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    vec3 viewPos, lookDir;
    camera_get_pos(cam, viewPos);
    camera_get_look_dir(cam, lookDir);
    vao_bind(cubeVao);
    shaderprogram_use(program);
    shaderprogram_set_vec3p(program, "viewPos", viewPos);
    update_lights(program, lights, 4);
    for (int i = 0; i < 10; i++) {
      glm_mat4_identity(model);
      glm_translate(model, positions[i]);
      glm_rotate(model, glm_rad(20.0f*i), (vec3){1.0f, 0.3f, 0.5f});
      shaderprogram_set_mat4(program, "model", model[0]);
      shaderprogram_set_mat4(program, "view", view[0]);
      shaderprogram_set_mat4(program, "proj", proj[0]);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    camera_set_speed(cam, move_speed);
    camera_set_mouse_sensitivity(cam, mouse_sensitivity);
    ImGui_Begin("Hello Window");
    camera_params(&rotation_speed, &move_speed, &fov, &mouse_sensitivity);
    // scene_params(lightColor, objectColor);
    // material_params(&shiny, &ambient, &diffuse, &specular);
    uint32_t n = 4;
    modify_scene_params(lights, n);
    ImGui_End();
    // demo();
    ImGui_Render();
    // printf("%d\n",n);

    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  ImGui_Deinit();
  glfwTerminate();
  return 0;
}

