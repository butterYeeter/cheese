#include "texture.h"

#include <stdbool.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <stb_image.h>
#include <string.h>


struct _Texture {
  uint32_t ID;
  char type[32];
  char path[256];
};

static GLint formats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
static char* tex_types[] = {"texture_diffuse", "texture_specular", "texture_none"};

static GLint get_texture_format(int num_channels) {
  if (1 <= num_channels && num_channels <= 4) return formats[num_channels - 1];
  return 0;
}

static char *textype_to_string(TextureType type) {
  if (0 <= type && type <= 2)
    return tex_types[type];
  return "texture_error";
}


Texture texture_create_from_mem(TextureType type, bool gen_mipmaps, int32_t width, int32_t height, int32_t num_channels, void *data) {
  Texture tex = malloc(sizeof(struct _Texture));
  memset(tex, 0, sizeof(struct _Texture));

  glGenTextures(1, &tex->ID);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex->ID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (gen_mipmaps) ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, get_texture_format(num_channels), width, height, 0, get_texture_format(num_channels), GL_UNSIGNED_BYTE, data);

  if (gen_mipmaps)
    glGenerateMipmap(GL_TEXTURE_2D);

  strncpy(tex->type, textype_to_string(type), 32);

  glBindTexture(GL_TEXTURE_2D, 0);
  return tex;
}

Texture texture_create_from_file(TextureType type, bool gen_mipmaps, const char *path) {
  int32_t width, height, num_channels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path, &width, &height, &num_channels, 0);

  Texture tex = texture_create_from_mem(type, gen_mipmaps, width, height, num_channels, data);
  stbi_image_free(data);

  strncpy(tex->path, path, 256);

  return tex;
}

uint32_t texture_get_id(Texture tex) {
  return tex->ID;
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