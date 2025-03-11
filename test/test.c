#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include "../include/cglm/cglm.h"
#include "../include/stb_image.h"
#include "GLFW/glfw3.h"
#include "fast_obj.h"
#define CGLTF_IMPLEMENTATION
#include "../include/cgltf.h"
#include "khash.h"
// #include "../include/shader.h"


typedef struct {
  uint32_t p;
  uint32_t t;
  uint32_t n;
} Key;


typedef struct {
  vec3 position;
  vec2 texcoord;
  vec3 normal;
} Vertex;


khint32_t cantorHash(int32_t a, int32_t b) {
  return (a + b + 1) * (a + b) / 2 + b;
}


khint32_t hasKey(Key k) {
  return cantorHash(k.p, cantorHash(k.t, k.n));
}


khint32_t hasKeyEqual(Key a, Key b) {
  if (memcmp(&a, &b, sizeof(Key))) {
    return true;
  }
  return false;
}

// Initialize a map structure with VertexKey as the key and the vertex index(uint32_t) as the value
KHASH_INIT(VertexMap, Key, uint32_t, true, hasKey, hasKeyEqual)

const char *read_file(const char *path) {
  FILE *file = fopen(path, "rb");
  size_t file_size = 0;
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);

  char *buffer = malloc(file_size + 1);
  buffer[file_size] = '\0';

  fread(buffer, 1, file_size, file);
  fclose(file);
  return (const char *)buffer;
}

GLuint create_shader(const char *path, int type) {
  const char *src = read_file(path);

  GLuint id = glCreateShader(type);
  glShaderSource(id, 1, &src, NULL);
  glCompileShader(id);
  return id;
}

int main() {
  glfwInit();
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *win = glfwCreateWindow(800, 600, "Tree load test", NULL, NULL);
  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to load GL functions\n");
    return 1;
  }
  glEnable(GL_DEPTH_TEST);

  fastObjMesh *m = fast_obj_read("backpack.obj");
  printf("Number of mesh objects: %d\n", m->object_count);
  printf("Number of mesh groups: %d\n", m->group_count);
  size_t buf_size = m->index_count * 8 * sizeof(float);
  float *buf = malloc(buf_size);
  for (int i = 0; i < m->index_count; i++) {
    buf[i * 8 + 0] = m->positions[m->indices[i].p * 3 + 0];
    buf[i * 8 + 1] = m->positions[m->indices[i].p * 3 + 1];
    buf[i * 8 + 2] = m->positions[m->indices[i].p * 3 + 2];
    buf[i * 8 + 3] = m->texcoords[m->indices[i].t * 2 + 0];
    buf[i * 8 + 4] = m->texcoords[m->indices[i].t * 2 + 1];
    buf[i * 8 + 5] = m->normals[m->indices[i].t * 3 + 0];
    buf[i * 8 + 6] = m->normals[m->indices[i].t * 3 + 1];
    buf[i * 8 + 7] = m->normals[m->indices[i].t * 3 + 2];
  }

  cgltf_options opt = {0};
  cgltf_data *data;
  cgltf_result ret = cgltf_parse_file(&opt, "Tree.glb", &data);

  if (ret != cgltf_result_success) {
    printf("Failed to load gltf\n");
    return 2;
  }

  ret = cgltf_load_buffers(&opt, data, "Tree.glb");

  if (ret != cgltf_result_success)
    return 3;

  cgltf_mesh *mesh = &data->meshes[0];
  cgltf_primitive *prim = &mesh->primitives[0];

  cgltf_attribute *pos_attr = NULL;
  cgltf_attribute *uv_attr = NULL;
  cgltf_attribute *norm_attr = NULL;

  for (size_t i = 0; i < prim->attributes_count; i++) {
    cgltf_attribute *attr = &prim->attributes[i];

    switch (attr->type) {
      case cgltf_attribute_type_position:
        pos_attr = attr;
        break;
      case cgltf_attribute_type_texcoord:
        uv_attr = attr;
        break;
      case cgltf_attribute_type_normal:
        norm_attr = attr;
        break;
      default:
        break;
    }
  }

  if (!pos_attr) {
    printf("Wtf No vertex positiosn in mesh\n");
    return 4;
  }

  cgltf_buffer_view *pos_buf_view = pos_attr->data->buffer_view;
  cgltf_buffer_view *uv_buf_view = uv_attr->data->buffer_view;

  if (uv_attr->data->buffer_view != pos_buf_view) {
    printf("Non Interleaved vertex data!\n");
    // return 5;
  }

  cgltf_buffer_view *index_buffer_view = prim->indices->buffer_view;
  uint16_t *arr = prim->indices->buffer_view->buffer->data + prim->indices->buffer_view->offset;

  GLuint vao, vbo, ebo;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBufferData(GL_ARRAY_BUFFER, pos_buf_view->size + uv_buf_view->size, NULL, GL_STATIC_DRAW);
  // glBufferSubData(GL_ARRAY_BUFFER, 0, pos_buf_view->size, pos_buf_view->buffer->data + pos_buf_view->offset);
  // glBufferSubData(GL_ARRAY_BUFFER, pos_buf_view->size, uv_buf_view->size, uv_buf_view->buffer->data + uv_buf_view->offset);
  // glGenBuffers(1, &ebo);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_view->size, (char*)index_buffer_view->buffer->data + index_buffer_view->offset, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, buf_size, buf, GL_STATIC_DRAW);


  cgltf_accessor *pos_accessor = pos_attr->data;
  cgltf_accessor *uv_accessor = uv_attr->data;
  // glEnableVertexAttribArray(0);
  // glVertexAttribPointer(0, cgltf_num_components(pos_accessor->type), GL_FLOAT, GL_FALSE, pos_accessor->stride ? pos_accessor->stride : 3 * sizeof(float), (void*)pos_accessor->offset);
  // glEnableVertexAttribArray(1);
  // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)1164);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));


  mat4 model, view, proj;
  glm_mat4_identity(model);
  glm_translate(model, (vec3){0.0f, -0.5f, 0.0f});
  glm_rotate(model, glm_rad(glfwGetTime()), (vec3){0.0f, 1.0f, 0.0f});
  glm_scale(model, (vec3){0.001f, 0.001f, 0.001f});
  glm_mat4_identity(view);
  glm_lookat((vec3){0.0f, 0.0f, 4.0f}, (vec3){0.0f, 0.0f, 1.0f}, (vec3){0.0f, 1.0f, 0.0f}, view);
  glm_mat4_identity(proj);
  glm_perspective(glm_rad(45.0f), (float)800/600, 0.1f, 100.0f, proj);
  GLuint v = create_shader("s.v.glsl", GL_VERTEX_SHADER);
  GLuint f = create_shader("s.f.glsl", GL_FRAGMENT_SHADER);
  GLuint p = glCreateProgram();
  glAttachShader(p, v);
  glAttachShader(p, f);
  glLinkProgram(p);
  glUseProgram(p);
  GLint model_loc = glGetUniformLocation(p, "model");
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);
  GLint view_loc = glGetUniformLocation(p, "view");
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, view[0]);
  GLint proj_loc = glGetUniformLocation(p, "proj");
  glUniformMatrix4fv(proj_loc, 1, GL_FALSE, proj[0]);


  cgltf_image *img = data->textures->image;
  cgltf_sampler *sampler = data->textures->sampler;

  int w,h,n;
  unsigned char *img_data = stbi_load_from_memory(img->buffer_view->buffer->data + img->buffer_view->offset, img->buffer_view->size, &w, &h, &n, 0);
  printf("%d, %d, %d\n", w, h, n);
  GLuint tex0;
  glGenTextures(1, &tex0);
  glBindTexture(GL_TEXTURE_2D, tex0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);

  cgltf_data *data2;
  cgltf_parse_file(&opt, "Mazda RX-7.glb", &data2);
  // cgltf_load_buffers(&opt, data2, )


  while (!glfwWindowShouldClose(win)) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm_mat4_identity(model);
    glm_translate(model, (vec3){0.0f, 0.0f, -10.0f});
    glm_rotate(model, glfwGetTime(), (vec3){0.0f, 1.0f, 0.0f});
    // glm_scale_uni(model, 0.002f);
    // glm_scale_uni(model, 2.0f);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, model[0]);
    // glDrawElements(GL_TRIANGLES, prim->indices->count, GL_UNSIGNED_SHORT, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, m->index_count);

    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  cgltf_free(data);
  glfwTerminate();
  return 0;
}
