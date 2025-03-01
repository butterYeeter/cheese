#include <vao.h>
#include <stdlib.h>

union _VAO {
  uint32_t ID;
};

void vao_create_new(VAO *vao) {
  *vao = malloc(sizeof(union _VAO));
  glGenVertexArrays(1, &(*vao)->ID);
  return;
}

void vao_enable_index(VAO vao, uint32_t index) {
  // glBindVertexArray(vao->ID);
  glEnableVertexAttribArray(index);
  // glBindVertexArray(0);
}

void vao_attrib_pointer(VAO vao, uint32_t index, int32_t size, uint32_t type, uint32_t normalized, size_t stride, void * offset) {
  // glBindVertexArray(vao->ID);
  glVertexAttribPointer(index, size, type, normalized, stride, offset);
  // glBindVertexArray(0);
}

void vao_bind(VAO vao) {
  glBindVertexArray(vao->ID);
}
