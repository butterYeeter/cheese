#ifndef EBO_H
#define EBO_H

#include <stdint.h>
#include <stddef.h>

typedef struct _EBO *EBO;

void ebo_create_new(EBO *ebo, uint32_t buffer_type);

void ebo_buffer_data(EBO ebo, size_t size, void *data, uint32_t usage);

void ebo_bind(EBO ebo);

void ebo_unbind();
#endif