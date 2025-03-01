#ifndef VBO_H
#define VBO_H

#include <stdint.h>
#include <stddef.h>

typedef struct _VBO *VBO;


void vbo_create_new(VBO *vbo, uint32_t buffer_type);

void vbo_buffer_data(VBO vbo, size_t size, void *data, uint32_t usage);

void vbo_buffer_subdata(long long offset, size_t size, void * data);

void vbo_bind(VBO vbo);

void vbo_unbind();

#endif