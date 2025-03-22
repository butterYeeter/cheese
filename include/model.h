#ifndef MODEL
#define MODEL

#include "shader.h"
#include "texture.h"
typedef struct _Model *Model;

void model_create(Model *m, char *path, Texture arr[2]);

void model_draw(Model m, ShaderProgram sp);

#endif /* MODEL */
