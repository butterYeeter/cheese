#ifndef MODEL
#define MODEL

#include "shader.h"
typedef struct _Model *Model;

void model_create(Model *m, const char *directory, const char *path);

void model_draw(Model m, ShaderProgram sp);

#endif /* MODEL */
