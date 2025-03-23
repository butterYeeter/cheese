#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct _Texture *Texture;

typedef enum {
  texture_diffuse = 0,
  texture_specular = 1,
  texture_none
} TextureType;

Texture texture_create_from_mem(TextureType type, bool gen_mipmaps, int32_t width, int32_t height, int32_t num_channels, void *data);

Texture texture_create_from_file(TextureType type, bool gen_mipmaps, const char *path);

uint32_t texture_get_id(Texture tex);

char *texture_get_type(Texture tex);

char *texture_get_path(Texture tex);

void texture_bind(Texture tex, uint8_t binding_slot);

void texture_unbind(uint8_t binding_slot);

#endif /* TEXTURE */
