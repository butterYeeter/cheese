#version 330 core

in vec2 uv;

uniform sampler2D screen;

out vec4 color;

void main() {
  color = texture(screen, uv);
  float average = (color.r + color.g + color.b) / 3.;
  color = vec4(vec3(average), 1.);
}