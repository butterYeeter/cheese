#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct _Texture *Texture;

typedef enum {
  texture_diffuse = 0,
  texture_specular = 1
} TextureType;

void texture_create(Texture *tex, uint8_t binding_slot, const char *path, TextureType type);

char *texture_get_type(Texture tex);

char *texture_get_path(Texture tex);

void texture_bind(Texture tex, uint8_t binding_slot);

void texture_unbind(uint8_t binding_slot);

#endif /* TEXTURE */
