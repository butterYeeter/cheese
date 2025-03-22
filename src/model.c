#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <khash.h>
#include <kvec.h>

KHASH_MAP_INIT_STR(TextureMap, Texture);

struct _Model {
  Mesh *meshes;
  uint32_t mesh_count;
  // Texture *textures_loaded;
  khash_t(TextureMap) *textures_loaded;
  uint32_t texture_count;
  char directory[256];
};

void model_load(Model m, char *path);
void model_process_node(Model m, struct aiNode *node, const struct aiScene *scene);
Mesh model_process_mesh(Model m, struct aiMesh *mesh, const struct aiScene *scene);
void model_load_material_textures(Model m, Texture array[32], struct aiMaterial *mat, enum aiTextureType type, uint32_t *count);

void model_create(Model *m, char *path) {
  // Allocate space for a model and zero it
  *m = malloc(sizeof(struct _Model));
  memset(*m, 0, sizeof(struct _Model));

  // Allocate space for atleast 1 mesh inside of a model
  (*m)->meshes = malloc(sizeof(Mesh));

  // Allocate space for texture cache. This array is used to prevent repeatedly
  // loading previously loaded textures
  // (*m)->textures_loaded = malloc(sizeof(Texture));
  // kv_init((*m)->textures_loaded);
  (*m)->textures_loaded = kh_init(TextureMap);

  // Call model load function to load mesh data
  model_load(*m, path);
}

void model_load(Model m, char *path) {
  // Use assimp to import model file into aiScene struct
  const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

  // Check to see that the scene was loaded correctly
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    const char *error = aiGetErrorString();
    printf("ASSIMP ERROR:: %s\n", error);
    return;
  }

  // Retrieve the base directory of the model file for loading assets later
  char *dir = strrchr(path, '/') + 1;
  strncpy(m->directory, path, strlen(path) - strlen(dir));
  printf("Model Directory: %s\n", m->directory);

  // Process scene root node and its children recursively
  model_process_node(m, scene->mRootNode, scene);
}

void model_process_node(Model m, struct aiNode *node, const struct aiScene *scene) {
  // Iterate over every mesh inside of a node and insert it into our model's mesh array
  for (uint32_t i = 0; i < node->mNumMeshes; i++) {
    // Retrieve assimp mesh and allocate space in our array
    printf("Processing mesh %d for node\n", i);
    struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    m->mesh_count++;
    m->meshes = realloc(m->meshes, m->mesh_count * sizeof(Mesh));

    // Call process_mesh to convert aiMesh structure into our custom mesh structure
    m->meshes[m->mesh_count - 1] = model_process_mesh(m, mesh, scene);
    for (uint32_t j = 0; j < kh_end(m->textures_loaded); j++) {
      printf("Texture loaded: %s\n", texture_get_path(kh_value(m->textures_loaded, j)));
    }
  }

  // Recursively call process_node for all child nodes
  for (uint32_t i = 0; i < node->mNumChildren; i++) {
    model_process_node(m, node->mChildren[i], scene);
    printf("Processing child node %d\n", i);
  }
}

Mesh model_process_mesh(Model m, struct aiMesh *mesh, const struct aiScene *scene) {
  // Allocate space for atleast 1 vertex, 1 index and 1 texture
  Vertex *vertices = malloc(sizeof(Vertex));
  uint32_t num_vertices = 0;
  uint32_t *indices = malloc(sizeof(uint32_t));
  uint32_t num_indices = 0;
  Texture *textures = malloc(32 * sizeof(Texture));
  uint32_t num_textures = 0;

  for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    vertex.position[0] = mesh->mVertices->x;
    vertex.position[1] = mesh->mVertices->y;
    vertex.position[2] = mesh->mVertices->z;
    vertex.normal[0] = mesh->mNormals->x;
    vertex.normal[1] = mesh->mNormals->y;
    vertex.normal[2] = mesh->mNormals->z;

    if (mesh->mTextureCoords[0]) {
      vertex.texcoord[0] = mesh->mTextureCoords[0][i].x;
      vertex.texcoord[1] = mesh->mTextureCoords[0][i].y;
    } else {
      vertex.texcoord[0] = 0.0f;
      vertex.texcoord[1] = 0.0f;
    }

    num_vertices++;
    vertices = realloc(vertices, num_vertices * sizeof(Vertex));
    vertices[num_vertices - 1] = vertex;
  }

  printf("Processed %d vertices for current mesh\n", num_vertices);

  for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
    struct aiFace face = mesh->mFaces[i];

    for (uint32_t j = 0; j < face.mNumIndices; j++) {
      num_indices++;
      indices = realloc(indices, num_indices * sizeof(uint32_t));
      indices[num_indices - 1] = face.mIndices[j];
    }
  }

  printf("Processed %d indices for current mesh\n", num_indices);

  if (mesh->mMaterialIndex >= 0) {
    struct aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
    uint32_t diffuse_texture_count;
    printf("Attempting to load diffuse texture maps\n");
    Texture diffuse_maps[32];
    model_load_material_textures(m, diffuse_maps, mat, aiTextureType_DIFFUSE, &diffuse_texture_count);
    for (khint_t j = kh_begin(m->textures_loaded); j < kh_end(m->textures_loaded); j++)
      printf("==================================\nTexture path loaded %s\n============================================\n", texture_get_path(kh_val(m->textures_loaded, j)));
    uint32_t specular_texture_count;
    printf("Attempting to load specular texture maps\n");
    Texture specular_maps[32];
    model_load_material_textures(m, specular_maps, mat, aiTextureType_SPECULAR, &specular_texture_count);
    for (int j = 0; j < kh_end(m->textures_loaded); j++)
      printf("===================================\nTexture path loaded %s\n============================================\n", texture_get_path(kh_val(m->textures_loaded, j)));

    for (uint32_t i = 0; i < diffuse_texture_count; i++) {
      // textures = realloc(textures, ++num_textures * sizeof(Texture));
      textures[num_textures++] = diffuse_maps[i];
    }

    for (uint32_t i = 0; i < specular_texture_count; i++) {
      // textures = realloc(textures, ++num_textures * sizeof(Texture));
      textures[diffuse_texture_count + num_textures++] = specular_maps[i];
    }

    printf("%d textures loaded for current mesh\n", num_textures);
  }

  Mesh _mesh;
  mesh_create(&_mesh, vertices, num_vertices, indices, num_indices, textures, num_textures);
  // free(vertices);
  // free(indices);
  // free(textures);
  return _mesh;
}

void model_load_material_textures(Model m, Texture array[32], struct aiMaterial *mat, enum aiTextureType type, uint32_t *count) {
  Texture *textures = malloc(sizeof(Texture));
  TextureType tex_type;
  uint32_t tex_count = 0;

  switch (type) {
    case aiTextureType_DIFFUSE:
      tex_type = texture_diffuse;
      break;
    case aiTextureType_SPECULAR:
      tex_type = texture_specular;
      break;
    default:
      tex_type = -1;
  }
  
  for (uint32_t i = 0; i < aiGetMaterialTextureCount(mat, type); i++) {
    struct aiString str;
    aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);
    bool skip = false;
    

    char path[256];
    strncpy(path, m->directory, 256);
    strncat(path, str.data, 256 - strlen(m->directory));
    printf("Loading Texture: %s\n", path);

    khint_t k;
    int ret;
    k = kh_put(TextureMap, m->textures_loaded, path, &ret);
    if (!ret) {
      printf("Already loaded. Simply copying cached texture into array\n");
      // textures = realloc(textures, ++tex_count * sizeof(Texture));
      array[tex_count++] = kh_value(m->textures_loaded, k);
      skip = true;
    }
    
    if (!skip) {
      Texture tex;
      printf("Texture has not been cached. Attempting to load\n");
      texture_create(&tex, 0, path, tex_type);
      // textures = realloc(textures, ++tex_count * sizeof(Texture));
      textures[tex_count++] = tex;
      printf("Texture cache count %d\n", kh_size(m->textures_loaded));
      // int ret;
      // khint_t k = kh_put(TextureMap, m->textures_loaded, path, &ret);
      kh_value(m->textures_loaded, k) = tex;
      // printf("textuer cache count 2 %d\n", kh_size(m->textures_loaded));
    }
  }
  
  *count = tex_count;
}

void model_draw(Model m, ShaderProgram sp) {
  // Iteratively draw all meshes contained within our model
  for (uint32_t i = 0; i < m->mesh_count; i++)
    mesh_draw(m->meshes[i], sp);
}