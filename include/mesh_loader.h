#ifndef MESH_LOADER
#define MESH_LOADER

#include <stdint.h>

void load_mesh_data(float *vertex_buffer, uint32_t * vertex_count, uint32_t *index_buffer, uint32_t *index_count, const char * path);

#endif /* MESH_LOADER */
