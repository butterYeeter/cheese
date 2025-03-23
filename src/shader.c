#include "shader.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "vao.h"
#include "util.h"

struct _Shader {
  uint32_t ID;
  uint32_t type;
};

struct _ShaderProgram {
  uint32_t ID;
  Shader vertex_shader;
  Shader fragmen_shader;
};


void compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (*type != 'P')
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			printf("SHADER_COMPILATION_ERROR for: %s\n%s", type, infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			printf("SHADER_LINKING_ERROR for: %s\n%s", type, infoLog);
		}
	}
}

Shader shader_create(uint32_t type, const char * const source) {
  Shader shader = malloc(sizeof(struct _Shader));
  shader->ID = glCreateShader(type);
  shader->type = type;
  // printf("%s\n", source);
  glShaderSource(shader->ID, 1, &source, NULL);
  glCompileShader(shader->ID);
  return shader;
}

ShaderProgram shaderprogram_create(const char *directory, const char *const vert_shader_path, const char *const frag_shader_path) {
  ShaderProgram program;
  Shader vertex, fragment;
  char *vertex_source, *fragment_source;
  char _vert_shader_path[256], _frag_shader_path[256];
  snprintf(_vert_shader_path, 256, "%s/%s", directory, vert_shader_path);
  snprintf(_frag_shader_path, 256, "%s/%s", directory, frag_shader_path);

  // Read shader sources using given paths
  if (ReadFile(_vert_shader_path, &vertex_source))
    return NULL;

  if (ReadFile(_frag_shader_path, &fragment_source))
    return NULL;

  // Create vertex and fragment shaders
  vertex = shader_create(GL_VERTEX_SHADER, vertex_source);
  compileErrors(vertex->ID, "SVert");
  fragment = shader_create(GL_FRAGMENT_SHADER, fragment_source);
  compileErrors(fragment->ID, "SFrag");

  // Allocate memory for ShaderProgram
  program = malloc(sizeof(struct _ShaderProgram));

  if (NULL == program)
    return NULL;


  // Create shader program and link with shaders
  program->ID = glCreateProgram();
  glAttachShader(program->ID, vertex->ID);
  glAttachShader(program->ID, fragment->ID);
  glLinkProgram(program->ID);

  compileErrors(program->ID, "P");

  // Clean up
  free(vertex_source);
  free(fragment_source);

  return program;
}

void shaderprogram_use(ShaderProgram program) { glUseProgram(program->ID); }

void shaderprogram_set_int(ShaderProgram program, const char * name, int value) {
  uint32_t location = glGetUniformLocation(program->ID, name);
  glUniform1i(location, value);
}

void shaderprogram_set_float(ShaderProgram program, const char * name, float value) {
  uint32_t location = glGetUniformLocation(program->ID, name);
  glUniform1f(location, value);
}

void shaderprogram_set_bool(ShaderProgram program, const char * name, bool value) {
  uint32_t location = glGetUniformLocation(program->ID, name);
  glUniform1i(location, value);
}

void shaderprogram_set_vec3(ShaderProgram program, const char * name, float v0, float v1, float v2) {
  uint32_t location = glGetUniformLocation(program->ID, name);
  glUniform3f(location, v0, v1, v2);
}

void shaderprogram_set_vec3p(ShaderProgram program, const char * name, float *vec) {
  uint32_t location = glGetUniformLocation(program->ID, name);
  glUniform3fv(location, 1, vec);
}

void shaderprogram_set_mat4(ShaderProgram program, const char * name, float *mat) {
  uint32_t location = glGetUniformLocation(program->ID, name);
  glUniformMatrix4fv(location, 1, GL_FALSE, mat);
}

uint32_t shaderprogram_getID(ShaderProgram program) {
  return program->ID;
}