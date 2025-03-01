#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include <stdint.h>
#include <stddef.h>

typedef union _VAO *VAO;


void vao_create_new(VAO *vao);

void vao_enable_index(VAO vao, uint32_t index);

void vao_attrib_pointer(VAO vao, uint32_t index, int32_t size, uint32_t type,
                        uint32_t normalized, size_t stride, void *offset);

void vao_bind(VAO vao);

#endif /* VAO */
