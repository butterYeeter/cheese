#ifndef CAMERA
#define CAMERA

#include "cglm/types.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

typedef struct _Camera *Camera;

void camera_create(Camera *cam, vec3 pos, float speed);

void camera_matrix(Camera cam, float FOVdeg, float near_plane, float far_plane);

void camera_get_matrix(Camera cam, mat4 matrix);

void camera_get_pos(Camera cam, vec3 pos);

void camera_get_look_dir(Camera cam, vec3 lookDir);

void camera_set_speed(Camera cam, float speed);

void camera_set_mouse_sensitivity(Camera cam, float mouse_sensitivity);

void camera_process_keys(Camera cam, GLFWwindow *window, float delta_time);

void camera_process_mouse_movement(Camera cam, float xoffset, float yoffset, float delta_time);

#endif /* CAMERA */
