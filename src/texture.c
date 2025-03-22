#include "texture.h"

#include <stdbool.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <stb_image.h>
#include <string.h>

// typedef enum _TextureFormat {
//   RGB = 3,
//   RGBA = 4
// } TextureFormat;

struct _Texture {
  uint32_t ID;
  char type[32];
  char path[256];
  // TextureFormat format;
};

GLint get_texture_format(int num_channels) {
  if (num_channels >= 1 && num_channels <= 4) {
    switch (num_channels) {
    case 1:
      return GL_RED;
    case 2:
      return GL_RG;
    case 3:
      return GL_RGB;
    case 4:
      return GL_RGBA;
    }
  }

  return 0;
}


void _texture_create(Texture tex, uint8_t binding_slot, const char *path, TextureType type) {
  glGenTextures(1, &tex->ID);
  glActiveTexture(GL_TEXTURE0 + binding_slot);
  glBindTexture(GL_TEXTURE_2D, tex->ID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  int32_t width, height, num_channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);
  printf("%d %d\n", num_channels, (num_channels+0x1904)-GL_RGBA);
  // if ((n | 0x1904) == GL_RGBA)
    // printf("Test\n");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, get_texture_format(num_channels), GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  switch (type) {
    case texture_diffuse:
      // tex->type = "texture_diffuse";
      strncpy(tex->type, "texture_diffuse", sizeof(tex->type));
      break;
    case texture_specular:
      // tex->type = "texture_specular";
      strncpy(tex->type, "texture_specular", sizeof(tex->type));
      break;
    default:
      // tex->type = "texture_error";
      strncpy(tex->type, "texture_error", sizeof(tex->type));
  }

  strncpy(tex->path, path, 256);
}

void texture_create(Texture *tex, uint8_t binding_slot, const char *path, TextureType type) {
  *tex = malloc(sizeof(struct _Texture));
  memset(*tex, 0, sizeof(struct _Texture));

  _texture_create(*tex, binding_slot, path, type);
}

char *texture_get_type(Texture tex) {
  return tex->type;
}

char *texture_get_path(Texture tex) {
  return tex->path;
}

void texture_bind(Texture tex, uint8_t binding_slot) {
  glActiveTexture(GL_TEXTURE0 + binding_slot);
  glBindTexture(GL_TEXTURE_2D, tex->ID);
}

void texture_unbind(uint8_t binding_slot) {
  glActiveTexture(GL_TEXTURE0 + binding_slot);
  glBindTexture(GL_TEXTURE_2D, 0);
}