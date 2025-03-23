#include <vao.h>
#include <stdlib.h>

union _VAO {
  uint32_t ID;
};

VAO vao_create_new() {
  VAO vao = malloc(sizeof(union _VAO));
  glGenVertexArrays(1, &vao->ID);
  return vao;
}

void vao_enable_index(VAO vao, uint32_t index) {
  glEnableVertexAttribArray(index);
}

void vao_attrib_pointer(VAO vao, uint32_t index, int32_t size, uint32_t type, uint32_t normalized, size_t stride, void * offset) {
  glVertexAttribPointer(index, size, type, normalized, stride, offset);
}

void vao_bind(VAO vao) {
  glBindVertexArray(vao->ID);
}

void vao_unbind() {
  glBindVertexArray(0);
}
