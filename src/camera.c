#include "camera.h"
#include "GLFW/glfw3.h"
#include "cglm/cam.h"
#include "cglm/vec3.h"


vec3 frontDefault = {0.0f, 0.0f, -1.0f};
vec3 worldUp = {0.0f, 1.0f, 0.0f};
vec3 defaultRight = {1.0f, 0.0f, 0.0f};

struct _Camera {
  vec3 pos;
  vec3 front;
  vec3 right;
  vec3 up;
  bool first_click;

  float yaw;
  float pitch;

  float speed;
  float old_speed;
  float mouse_sensitivity;
};

void movement(Camera cam, vec3 dir, bool negate, float delta_time, float speed);

void update_camera_vectors(Camera cam);

void camera_create(Camera *cam, vec3 pos, float speed) {
  *cam = malloc(sizeof(struct _Camera));
  glm_vec3_dup(frontDefault, (*cam)->front);
  glm_vec3_dup(worldUp, (*cam)->up);
  glm_vec3_dup(pos, (*cam)->pos);
  glm_vec3_dup(defaultRight, (*cam)->right);
  (*cam)->pitch = 0.0f;
  (*cam)->yaw = -90.0f;
  (*cam)->speed = speed;
  (*cam)->old_speed = (*cam)->speed;
  (*cam)->mouse_sensitivity = 10.0f;
}


void camera_get_matrix(Camera cam, mat4 matrix) {
  vec3 temp;
  glm_vec3_add(cam->pos, cam->front, temp);
  // printf("direction: %.2f x %.2f y %.2f z\n", temp[0], temp[1], temp[2]);
  glm_lookat(cam->pos, temp, cam->up, matrix);
}

void camera_get_pos(Camera cam, vec3 pos) {
  glm_vec3_dup(cam->pos, pos);
}

void camera_get_look_dir(Camera cam, vec3 lookDir) {
  glm_vec3_dup(cam->front, lookDir);
}

void camera_set_speed(Camera cam, float speed) {
  // cam->speed = speed;
  cam->old_speed = speed;
}

void camera_set_mouse_sensitivity(Camera cam, float mouse_sensitivity) {
  cam->mouse_sensitivity = mouse_sensitivity;
}

void camera_process_keys(Camera cam, GLFWwindow *window, float delta_time) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    vec3 front;
    glm_vec3_cross(cam->right, worldUp, front);
    glm_vec3_normalize(front);
    movement(cam, front, false, delta_time, cam->speed);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    vec3 front;
    glm_vec3_cross(cam->right, worldUp, front);
    glm_vec3_normalize(front);
    movement(cam, front, true, delta_time, cam->speed);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    vec3 right;
    glm_vec3_cross(cam->front, cam->up, right);
    glm_vec3_normalize(right);
    movement(cam, right, true, delta_time, cam->speed);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    vec3 right;
    glm_vec3_cross(cam->front, cam->up, right);
    glm_vec3_normalize(right);
    movement(cam, right, false, delta_time, cam->speed);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    movement(cam, worldUp, false, delta_time, 3.0f);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
    movement(cam, worldUp, true, delta_time, 3.0f);
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    // cam->old_speed = cam->speed;
    cam->speed = 2.5*cam->old_speed;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
    cam->speed = cam->old_speed;
  }
}

void movement(Camera cam, vec3 dir, bool negate, float delta_time, float speed) {
  vec3 tmp, tmpdir;
  glm_vec3_dup(dir, tmpdir);
  if (negate)
    glm_vec3_negate(tmpdir);

  glm_vec3_scale(tmpdir, speed * delta_time, tmpdir);
  glm_vec3_add(cam->pos, tmpdir, cam->pos);
}

void camera_process_mouse_movement(Camera cam, float xoffset, float yoffset, float delta_time) {
  xoffset *= cam->mouse_sensitivity;
  yoffset *= cam->mouse_sensitivity;

  cam->yaw += xoffset * delta_time;
  cam->pitch += yoffset * delta_time;

  if (cam->pitch > 89.0f)
    cam->pitch = 89.0f;
  if (cam->pitch < -89.0f)
    cam->pitch = -89.0f;

  update_camera_vectors(cam);
}

void update_camera_vectors(Camera cam) {
  vec3 front;
  front[0] = cosf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
  front[1] = sinf(glm_rad(cam->pitch));
  front[2] = sinf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch));
  glm_vec3_normalize_to(front, cam->front);

  vec3 tmp;
  glm_vec3_cross(worldUp, cam->front, tmp);
  glm_vec3_normalize_to(tmp, cam->right);
  glm_vec3_cross(cam->front, cam->right, tmp);
  glm_vec3_normalize_to(tmp, cam->up);
}