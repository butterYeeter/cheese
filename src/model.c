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

typedef kvec_t(Texture) TextureVector;

struct _Model {
  Mesh *meshes;
  uint32_t mesh_count;
  khash_t(TextureMap) * textures_loaded;
  Texture *arr;
  uint32_t texture_count;
  char directory[256];
};

static void model_load(Model m, char *path);
static void model_process_node(Model m, struct aiNode *node, const struct aiScene *scene);
static Mesh model_process_mesh(Model m, struct aiMesh *mesh, const struct aiScene *scene);
static TextureVector model_load_material_textures(Model m, struct aiMaterial *mat, enum aiTextureType type);

void model_create(Model *m, char *path) {
  // Allocate space for a model and zero it
  *m = malloc(sizeof(struct _Model));
  memset(*m, 0, sizeof(struct _Model));

  // Allocate space for atleast 1 mesh inside of a model
  (*m)->meshes = malloc(sizeof(Mesh));

  // Allocate space for texture cache. This array is used to prevent repeatedly loading previously loaded textures
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
    struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    m->mesh_count++;
    m->meshes = realloc(m->meshes, m->mesh_count * sizeof(Mesh));

    // Call process_mesh to convert aiMesh structure into our custom mesh structure
    m->meshes[m->mesh_count - 1] = model_process_mesh(m, mesh, scene);
  }

  // Recursively call process_node for all child nodes
  for (uint32_t i = 0; i < node->mNumChildren; i++) {
    model_process_node(m, node->mChildren[i], scene);
  }
}

Mesh model_process_mesh(Model m, struct aiMesh *mesh, const struct aiScene *scene) {
  // Initilize arrays to store our data
  Vertex *vertices = malloc(sizeof(Vertex));
  uint32_t num_vertices = 0;
  uint32_t *indices = malloc(sizeof(uint32_t));
  uint32_t num_indices = 0;
  TextureVector textures;
  kv_init(textures);

  for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    vertex.position[0] = mesh->mVertices[i].x;
    vertex.position[1] = mesh->mVertices[i].y;
    vertex.position[2] = mesh->mVertices[i].z;
    vertex.normal[0] = mesh->mNormals[i].x;
    vertex.normal[1] = mesh->mNormals[i].y;
    vertex.normal[2] = mesh->mNormals[i].z;

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


  for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
    struct aiFace face = mesh->mFaces[i];

    for (uint32_t j = 0; j < face.mNumIndices; j++) {
      num_indices++;
      indices = realloc(indices, num_indices * sizeof(uint32_t));
      indices[num_indices - 1] = face.mIndices[j];
    }
  }


  if (mesh->mMaterialIndex >= 0) {
    struct aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
    TextureVector diffuse_maps = model_load_material_textures(m, mat, aiTextureType_DIFFUSE);
    for (uint32_t i = 0; i < kv_size(diffuse_maps); i++)
      kv_push(Texture, textures, kv_A(diffuse_maps, i));
    TextureVector specular_maps = model_load_material_textures(m, mat, aiTextureType_SPECULAR);
    for (uint32_t i = 0; i < kv_size(specular_maps); i++)
      kv_push(Texture, textures, kv_A(specular_maps, i));
  }

  Mesh _mesh;
  mesh_create(&_mesh, vertices, num_vertices, indices, num_indices, textures.a, kv_size(textures));
  return _mesh;
}

TextureVector model_load_material_textures(Model m, struct aiMaterial *mat, enum aiTextureType type) {
  TextureVector vector;
  TextureType tex_type;

  kv_init(vector);

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

    khint_t k;
    int ret;
    k = kh_put(TextureMap, m->textures_loaded, path, &ret);
    if (!ret) {
      kv_push(Texture, vector, kh_value(m->textures_loaded, k));
      skip = true;
    }
    
    if (!skip) {
      Texture tex;
      texture_create(&tex, 0, path, tex_type);
      kv_push(Texture, vector, tex);
      kh_value(m->textures_loaded, k) = tex;
      printf("Texture cache count %d\n", kh_size(m->textures_loaded));
    }
  }

  return vector;
}

void model_draw(Model m, ShaderProgram sp) {
  // Iteratively draw all meshes contained within our model
  for (uint32_t i = 0; i < m->mesh_count; i++)
    mesh_draw(m->meshes[i], sp);
}