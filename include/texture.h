#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>

typedef struct _Texture *Texture;

void texture_create(Texture *tex, uint8_t binding_slot, const char *path);

void texture_bind(Texture tex, uint8_t binding_slot);

void texture_unbind(uint8_t binding_slot);

#endif /* TEXTURE */
