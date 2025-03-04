#include <stdint.h>
#include <stdbool.h>
#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"
#include "khash.h"
#include <cglm/cglm.h>


typedef struct {
  fastObjUInt p;
  fastObjUInt t;
  fastObjUInt n;
} VertexKey;


typedef struct {
  vec3 position;
  vec2 texcoord;
  vec3 normal;
} Vertex;


khint32_t cantorHash(int32_t a, int32_t b) {
  return (a + b + 1) * (a + b) / 2 + b;
}


khint32_t hashVertexKey(VertexKey k) {
  return cantorHash(k.p, cantorHash(k.t, k.n));
}


khint32_t hashVertexKeyEqual(VertexKey a, VertexKey b) {
  if (memcmp(&a, &b, sizeof(VertexKey))) {
    return true;
  }
  return false;
}

// Initialize a map structure with VertexKey as the key and the vertex index(uint32_t) as the value
KHASH_INIT(VertexMap, VertexKey, uint32_t, true, hashVertexKey, hashVertexKeyEqual)


int main() {
  // Open and parse obj mesh
  fastObjMesh *mesh = fast_obj_read("backpack.obj");

  // Calculate maximum size of each attribute(assuming that there is ZERO repeating data)
  size_t positions_size = mesh->index_count * 3 * sizeof(float);
  size_t texcoords_size = mesh->index_count * 2 * sizeof(float);
  size_t normals_size   = mesh->index_count * 3 * sizeof(float);
  printf("Pos Size: %luB TexCoord Size: %luB Normal Size: %luB\n", positions_size, texcoords_size, normals_size);

  // Allocate vertex and index buffers 
  float * vertex_buffer = malloc(positions_size + texcoords_size + normals_size);
  uint32_t * index_buffer  = malloc(mesh->index_count * sizeof(uint32_t));
  // printf("Index Buffer Pointer: %p\n", mesh);
  // printf("Index Buffer Size: %lx\n", mesh->index_count * sizeof(uint32_t));
  printf("%u\n", mesh->index_count);

  uint32_t vertex_count = 0;
  for (uint32_t i = 0; i < mesh->index_count; i++) {
    // float *pos = mesh->positions + mesh->indices[i].p;
    // float *uv = mesh->positions + mesh->indices[i].p;
    // float *norm = mesh->positions + mesh->indices[i].p;

    // glm_vec3_copy(pos, vertex_buffer + vertex_count * sizeof(Vertex) + offsetof(Vertex, position));
    // glm_vec2_copy(uv, vertex_buffer + vertex_count * sizeof(Vertex) + offsetof(Vertex, texcoord));
    // glm_vec3_copy(norm, vertex_buffer + vertex_count * sizeof(Vertex) + offsetof(Vertex, normal));
    // vertex_count++;
    size_t offset = vertex_count * sizeof(Vertex) + offsetof(Vertex, normal);
    *(vertex_buffer + offset + 0) = 1.0f;
    *(vertex_buffer + offset + 1) = 2.0f;
    *(vertex_buffer + offset + 2) = 3.0f;
    printf("%.2f, %.2f, %.2f\n", *(vertex_buffer + offset), *(vertex_buffer + offset + 1), *(vertex_buffer + offset + 2));
  }





  // // Create a map structure
  // khash_t(VertexMap) *h = kh_init(VertexMap);
  // // kh_resize(VertexMap, h, mesh->index_count);

  // // Keep track of the total number of non-repeating vertices and indices written
  // uint32_t vertex_count = 0;
  // uint32_t index_count  = 0;



  // // Iterate over index array from fast_obj
  // for (uint32_t i = 0; i < mesh->index_count; i++) {
  // 	// if(67557 == i) printf("Reached problem\n");
  //   // printf("Index Count : %u, Loop Iteration: %u\n", index_count, i);

  //   // Get vertex key
  //   VertexKey key = {mesh->indices[i].p, mesh->indices[i].t, mesh->indices[i].n};
  //   int ret;
  //   khint_t k;

  //   // Attempt to insert vertex key into the map
  //   k = kh_put(VertexMap, h, key, &ret);

  //   // If the key already exists in the map, delete the current entry
  //   if (!ret) {
  //     // kh_del(VertexMap, h, k);

  //     // Write the existing index position of this vertex into the index buffer
  //     k = kh_get(VertexMap, h, key);
  //     index_buffer[index_count] = (uint32_t)kh_val(h, k);

  //     // Increment index count
  //     index_count++;
  //   } else {
  //     // Key hasn't been inserted yet


  //     // Retrieve vertex data from fastObjMesh structure
  //     float* position = (mesh->positions + key.p * 12);
  //     float* texcoord = (mesh->texcoords + key.t * 8);
  //     float* normal   = (mesh->normals + key.n * 12);

 
  //     // Copy interleaved vertex data into vertex buffer
  //     glm_vec3_copy(position, vertex_buffer + vertex_count * sizeof(Vertex) + offsetof(Vertex, position));
  //     glm_vec2_copy(texcoord, vertex_buffer + vertex_count * sizeof(Vertex) + offsetof(Vertex, texcoord));
  //     glm_vec3_copy(normal, vertex_buffer + vertex_count * sizeof(Vertex) + offsetof(Vertex, normal));

  //     // Set the index for this specific vertex in the map
  //     kh_val(h, k) = vertex_count;

  //     // Update index buffer
  //     index_buffer[index_count] = vertex_count;

  //     // Increment vertex count and index count
  //     vertex_count++;
  //     index_count++;
  //   }
  // }

  // // for (int i = 0; i < index_count; i++) {
  // //   printf("indexnum: %d index val: %u\n", i, index_buffer[i]);
  // // }

  // printf("Obj Refactor Summary:\n");
  // printf("Unique Vertex Count: %u\n", vertex_count);
  // printf("Number of indices generated: %u\n", index_count);

  fast_obj_destroy(mesh);
  // kh_destroy(VertexMap, h);i
  free(vertex_buffer);
  free(index_buffer);
  return 0;
}
