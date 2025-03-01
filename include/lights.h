#ifndef LIGHTS
#define LIGHTS

#include <cglm/types.h>
#include "shader.h"

typedef struct _PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
} PointLight;

void init_lights(PointLight lights[], uint32_t num_lights);
void update_lights(ShaderProgram program, PointLight lights[], uint32_t num_lights);

#endif /* LIGHTS */
