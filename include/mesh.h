#ifndef MESH
#define MESH

#include "cglm/types.h"
#include "shader.h"
#include "texture.h"
#include <stdint.h>

typedef struct _Mesh *Mesh;

typedef struct _Vertex {
  vec3 position;
  vec2 texcoord;
  vec3 normal;
} Vertex;

void mesh_create(Mesh *m, Vertex *vertices, size_t num_vertices, uint32_t *indices, size_t num_indices, Texture *textures, size_t num_textures);

void mesh_draw(Mesh m, ShaderProgram sp);

float *load_mesh_data(size_t *buf_size, uint32_t *vertex_count, const char * path);

float *load_mesh_data_indexed(size_t *vtx_buf_size, uint32_t *vtx_count, uint32_t **idx_buf, size_t *idx_buf_size, uint32_t *idx_count, const char * path);

#endif /* MESH */
