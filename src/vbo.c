#include "vbo.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>


// typedef struct _VBO *VBO;
struct _VBO {
  uint32_t ID;
  size_t size;
  uint32_t type;
};

void vbo_create_new(VBO *vbo, uint32_t buffer_type) {
  *vbo = malloc(sizeof(struct _VBO));
  memset(*vbo, 0, sizeof(struct _VBO));
  (*vbo)->type = buffer_type;
  glGenBuffers(1, &(*vbo)->ID);
  glBindBuffer(GL_ARRAY_BUFFER, (*vbo)->ID);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vbo_buffer_data(VBO vbo, size_t size, void * data, uint32_t usage) {
  // glBindBuffer(vbo->type, vbo->ID);
  glBufferData(GL_ARRAY_BUFFER, size, data, usage);
  // glBindBuffer(vbo->type, 0);
}

void vbo_buffer_subdata(long long offset, size_t size, void * data) {
  glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void vbo_bind(VBO vbo) {
  // if(vbo)
    glBindBuffer(GL_ARRAY_BUFFER, vbo->ID);
}

void vbo_unbind() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}