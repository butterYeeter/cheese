#version 330 core

in vec2 uv;

uniform sampler2D screen;

out vec4 color;

void main() {
  color = texture(screen, uv);
  float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
  color = vec4(vec3(average), 1.);
}