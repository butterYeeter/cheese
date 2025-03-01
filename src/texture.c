#include "texture.h"

#include <stdbool.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <stb_image.h>

// typedef enum _TextureFormat {
//   RGB = 3,
//   RGBA = 4
// } TextureFormat;

struct _Texture {
  uint32_t ID;
  int32_t width;
  int32_t height;
  // TextureFormat format;
};


void tex_create(Texture tex, uint8_t binding_slot, const char *path) {
  glGenTextures(1, &tex->ID);
  glActiveTexture(GL_TEXTURE0 + binding_slot);
  glBindTexture(GL_TEXTURE_2D, tex->ID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  int32_t n;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path, &tex->width, &tex->height, &n, 0);
  printf("%d %d\n", n, (n+6404)-GL_RGBA);
  if ((n | 0x1904) == GL_RGBA)
    printf("Test\n");
  glTexImage2D(GL_TEXTURE_2D, 0, n + 0x1904, tex->width, tex->height, 0, n + 0x1904, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void texture_create(Texture *tex, uint8_t binding_slot, const char *path) {
  *tex = malloc(sizeof(struct _Texture));

  tex_create(*tex, binding_slot, path);
}

void texture_bind(Texture tex, uint8_t binding_slot) {
  glActiveTexture(GL_TEXTURE0 + binding_slot);
  glBindTexture(GL_TEXTURE_2D, tex->ID);
}

void texture_unbind(uint8_t binding_slot) {
  glActiveTexture(GL_TEXTURE0 + binding_slot);
  glBindTexture(GL_TEXTURE_2D, 0);
}