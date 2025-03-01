#include "ebo.h"
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>


struct _EBO {
  uint32_t ID;
  size_t size;
  uint32_t type;
};


void ebo_create_new(EBO *ebo, uint32_t buffer_type) {
  *ebo = malloc(sizeof(struct _EBO));
  memset(*ebo, 0, sizeof(struct _EBO));
  (*ebo)->type = buffer_type;
  glGenBuffers(1, &(*ebo)->ID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*ebo)->ID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ebo_buffer_data(EBO ebo, size_t size, void *data, uint32_t usage) {
  glBufferData(ebo->type, size, data, usage);
}

void ebo_bind(EBO ebo) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->ID);
}

void ebo_unbind() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}