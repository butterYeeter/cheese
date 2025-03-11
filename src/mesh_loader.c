#include "mesh_loader.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
// #define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"
#include "khash.h"
#include <cglm/cglm.h>


typedef struct {
  fastObjUInt p;
  fastObjUInt t;
  fastObjUInt n;
} Key;


typedef struct {
  vec3 position;
  vec2 texcoord;
  vec3 normal;
} Vertex;


khint32_t cantorHash(int32_t a, int32_t b) {
  return (a + b + 1) * (a + b) / 2 + b;
}


khint32_t hashKey(Key k) {
  return cantorHash(k.p, cantorHash(k.t, k.n));
}


khint32_t hashKeyEqual(Key a, Key b) {
  if (!memcmp(&a, &b, sizeof(Key))) {
    return true;
  }
  return false;
}

static inline khint_t key_hash(Key k) {
    // Mix the three 32-bit integers into a single hash
    khint_t h = k.p;
    h = (h << 16) ^ (h >> 16) ^ k.t; // Shuffle bits
    h = (h * 16777619U) ^ k.n;       // FNV prime multiplier
    return h;
}

static inline int key_equal(Key a, Key b) {
    return (a.p == b.p) && (a.t == b.t) && (a.n == b.n);
}

// Initialize a map structure with VertexKey as the key and the vertex index(uint32_t) as the value
KHASH_INIT(VertexMap, Key, uint32_t, 1, key_hash, key_equal)


void beans(float *vertex_buffer, uint32_t * vertex_count, uint32_t *index_buffer, uint32_t *index_count, const char * path) {
  // Open and parse obj mesh
  fastObjMesh *mesh = fast_obj_read(path);
  printf("Read obj: %p\n", mesh);

  // Calculate maximum size of each attribute(assuming that there is ZERO repeating data)
  size_t positions_size = mesh->index_count * 3 * sizeof(float);
  size_t texcoords_size = mesh->index_count * 2 * sizeof(float);
  size_t normals_size   = mesh->index_count * 3 * sizeof(float);

  // Allocate vertex and index buffers 
  vertex_buffer = malloc(positions_size + texcoords_size + normals_size);
  index_buffer  = malloc(mesh->index_count * sizeof(uint32_t));
  printf("%u\n", mesh->index_count);
  
  
  // Create a map structure
  khash_t(VertexMap) *h = kh_init(VertexMap);
  // kh_resize(VertexMap, h, mesh->index_count);
  
  // Keep track of the total number of non-repeating vertices and indices written
  uint32_t _vertex_count = 0;
  uint32_t _index_count  = 0;
  
  
  
  // Iterate over index array from fast_obj
  for (uint32_t i = 0; i < mesh->index_count; i++) {
    // Get vertex key
    Key key = {mesh->indices[i].p, mesh->indices[i].t, mesh->indices[i].n};
    int ret;
    khint_t k;

    // Attempt to insert vertex key into the map
    k = kh_put(VertexMap, h, key, &ret);

    // If the key already exists in the map, delete the current entry
    if (!ret) {
      // kh_del(VertexMap, h, k);

      // Write the existing index position of this vertex into the index buffer
      k = kh_get(VertexMap, h, key);
      index_buffer[_index_count] = (uint32_t)kh_val(h, k);

      // Increment index count
      _index_count++;
    } else {
      // Key hasn't been inserted yet


      // Retrieve vertex data from fastObjMesh structure
      float* position = (mesh->positions + key.p);
      float* texcoord = (mesh->texcoords + key.t);
      float* normal   = (mesh->normals + key.n);

 
      // Copy interleaved vertex data into vertex buffer
      glm_vec3_copy(position, vertex_buffer + _vertex_count + 0);
      glm_vec2_copy(texcoord, vertex_buffer + _vertex_count + 3);
      glm_vec3_copy(normal, vertex_buffer + _vertex_count + 5);

      // Set the index for this specific vertex in the map
      kh_val(h, k) = _vertex_count;

      // Update index buffer
      index_buffer[_index_count] = _vertex_count;

      // Increment vertex count and index count
      _vertex_count++;
      _index_count++;
    }
  }

  // Return vertex and index count to user
  *vertex_count = _vertex_count - 1;
  *index_count = _index_count - 1;

  // Print out summary of mesh refactor
  printf("Obj Refactor Summary:\n");
  printf("Unique Vertex Count: %u\n", _vertex_count - 1);
  printf("Number of indices generated: %u\n", _index_count - 1);

  fast_obj_destroy(mesh);
  kh_destroy(VertexMap, h);
}

void copy_vtx_to_buf(float *buf, fastObjMesh *mesh, const uint32_t i) {
  fastObjIndex index = mesh->indices[i];
  buf[i * 8 + 0] = mesh->positions[index.p * 3 + 0];
  buf[i * 8 + 1] = mesh->positions[index.p * 3 + 1];
  buf[i * 8 + 2] = mesh->positions[index.p * 3 + 2];
  buf[i * 8 + 3] = mesh->texcoords[index.t * 2 + 0];
  buf[i * 8 + 4] = mesh->texcoords[index.t * 2 + 1];
  buf[i * 8 + 5] = mesh->normals[index.n * 3 + 0];
  buf[i * 8 + 6] = mesh->normals[index.n * 3 + 1];
  buf[i * 8 + 7] = mesh->normals[index.n * 3 + 2];
}

float *load_mesh_data(size_t *buf_size, uint32_t *vertex_count, const char * path) {
  fastObjMesh *mesh = fast_obj_read(path);

  if (NULL == mesh)
    return NULL;

  size_t buffer_size = mesh->index_count * 8 * sizeof(float);
  float *buffer = malloc(buffer_size);

  for (uint32_t i = 0; i < mesh->index_count; i++) {
    copy_vtx_to_buf(buffer, mesh, i);
  }

  *buf_size = buffer_size;
  *vertex_count = mesh->index_count;

  fast_obj_destroy(mesh);
  return buffer;
}

float *hashing_test(size_t *vtx_buf_size, uint32_t *vtx_count, uint32_t **idx_buf, size_t *idx_buf_size, uint32_t *idx_count, const char * path) {
  fastObjMesh *mesh = fast_obj_read(path);
  khash_t(VertexMap) *map = kh_init(VertexMap);

  if (NULL == mesh)
    return NULL;

  size_t vtx_buffer_size = mesh->index_count * 8 * sizeof(float);
  float *vtx_buffer = malloc(vtx_buffer_size);
  size_t idx_buffer_size = mesh->index_count * sizeof(uint32_t);
  uint32_t *idx_buffer = calloc(idx_buffer_size, 1);

  uint32_t vertex_count = 0;


  for (uint32_t i = 0; i < mesh->index_count; i++) {
    fastObjIndex idx = mesh->indices[i];
    Key key = {idx.p, idx.t, idx.n};

    int ret;
    khint32_t k = kh_put(VertexMap, map, key, &ret);

    if (ret == 1) {   // First occurence of this key
      // set index
      kh_value(map, k) = vertex_count;

      // Copy unique vertex combo to vertex buffer
      vtx_buffer[vertex_count * 8 + 0] = mesh->positions[idx.p * 3 + 0];
      vtx_buffer[vertex_count * 8 + 1] = mesh->positions[idx.p * 3 + 1];
      vtx_buffer[vertex_count * 8 + 2] = mesh->positions[idx.p * 3 + 2];
      vtx_buffer[vertex_count * 8 + 3] = mesh->texcoords[idx.t * 2 + 0];
      vtx_buffer[vertex_count * 8 + 4] = mesh->texcoords[idx.t * 2 + 1];
      vtx_buffer[vertex_count * 8 + 5] = mesh->normals[idx.n * 3 + 0];
      vtx_buffer[vertex_count * 8 + 6] = mesh->normals[idx.n * 3 + 1];
      vtx_buffer[vertex_count * 8 + 7] = mesh->normals[idx.n * 3 + 2];

      // Set current index
      idx_buffer[i] = kh_value(map, k);

      // Increment unique vertex count
      vertex_count++;
    } else if (ret == 0) {  // Key already exists in map
      // Simply set index of existing vertex
      idx_buffer[i] = kh_value(map, k);
    }
  }

  // Return data for user
  *vtx_buf_size = vtx_buffer_size;
  *vtx_count = vertex_count;
  *idx_buf = idx_buffer;
  *idx_buf_size = idx_buffer_size;
  *idx_count = mesh->index_count;

  // Cleanup
  fast_obj_destroy(mesh);
  kh_destroy(VertexMap, map);
  return vtx_buffer;
}