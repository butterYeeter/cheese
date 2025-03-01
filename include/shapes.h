#ifndef SHAPES
#define SHAPES

#include "cglm/types.h"
#include <stdint.h>
#include <stddef.h>

extern vec3 positions[];
extern size_t sizeof_positions;

extern vec3 lightPositions[];
extern size_t sizeof_lightPositions;

extern float triangle[];
extern size_t sizeof_triangle;

extern float square[];
extern size_t sizeof_square;

extern float tri_base_pyramid[];
extern size_t sizeof_tri_base_pyramid;

extern float tbp_colors[];
extern size_t sizeof_tbp_colors;

extern float cube[];
extern size_t sizeof_cube;

extern float cube_tex_coords[];
extern size_t sizeof_cube_tex_coords;

extern uint32_t tbp_indices[];
extern size_t sizeof_tbp_indices;

extern float cube_normals[];
extern size_t sizeof_cube_normals;

#endif /* SHAPES */
