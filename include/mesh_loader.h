#ifndef MESH_LOADER
#define MESH_LOADER

#include <stdint.h>
#include <stddef.h>

float *load_mesh_data(size_t *buf_size, uint32_t *vertex_count, const char * path);

float *hashing_test(size_t *vtx_buf_size, uint32_t *vtx_count, uint32_t **idx_buf, size_t *idx_buf_size, uint32_t *idx_count, const char * path);

#endif /* MESH_LOADER */
