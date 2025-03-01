#include "lights.h"
#include "shapes.h"
#include <cglm/cglm.h>
#include <string.h>


void init_lights(PointLight lights[], uint32_t num_lights) {
  for (int i = 0; i < num_lights; i++) {
    glm_vec3_copy(lightPositions[i], lights[i].position);
    glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, lights[i].ambient);
    glm_vec3_copy((vec3){0.5f, 0.5f, 0.5f}, lights[i].diffuse);
    glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, lights[i].specular);
    lights[i].constant = 1.0f;
    lights[i].linear = 0.005f;
    lights[i].quadratic = 0.00123f;
  }
}

void update_lights(ShaderProgram program, PointLight lights[], uint32_t num_lights) {
  for (int i = 0; i < num_lights; i++) {
    char buffer[64];

    uint32_t prefix_len = snprintf(buffer, sizeof(buffer), "pointLights[%u].", i);

    strcpy(buffer + prefix_len, "position");
    shaderprogram_set_vec3p(program, buffer, lights[i].position);

    strcpy(buffer + prefix_len, "ambient");
    shaderprogram_set_vec3p(program, buffer, lights[i].ambient);

    strcpy(buffer + prefix_len, "diffuse");
    shaderprogram_set_vec3p(program, buffer, lights[i].diffuse);

    strcpy(buffer + prefix_len, "specular");
    shaderprogram_set_vec3p(program, buffer, lights[i].specular);

    strcpy(buffer + prefix_len, "constant");
    shaderprogram_set_float(program, buffer, lights[i].constant);

    strcpy(buffer + prefix_len, "linear");
    shaderprogram_set_float(program, buffer, lights[i].linear);

    strcpy(buffer + prefix_len, "quadratic");
    shaderprogram_set_float(program, buffer, lights[i].quadratic);
  }
}