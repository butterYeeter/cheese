#version 330 core

vec3 border_color = vec3(0.3, 0.1, 0.5);

out vec4 fragColor;

void main() {
  fragColor = vec4(border_color, 1.0);
}