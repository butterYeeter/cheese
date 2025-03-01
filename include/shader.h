#ifndef SHADER
#define SHADER

#include <stdint.h>


typedef struct _ShaderProgram *ShaderProgram;

typedef struct _Shader *Shader;


void shader_create(Shader *shader, uint32_t type, const char * const source);

void shaderprogram_create(ShaderProgram *program,
                          const char *const vert_shad_pth,
                          const char *const frag_shad_pth);

void shaderprogram_use(ShaderProgram program);

void shaderprogram_set_int(ShaderProgram program, const char * name, int value);

void shaderprogram_set_float(ShaderProgram program, const char * name, float value);

void shaderprogram_set_vec3(ShaderProgram program, const char * name, float v0, float v1, float v2);

void shaderprogram_set_vec3p(ShaderProgram program, const char * name, float *vec);

void shaderprogram_set_mat4(ShaderProgram program, const char * name, float *mat);

uint32_t shaderprogram_getID(ShaderProgram program);

#endif /* SHADER */
