#version 330 core

in vec2 uv;

uniform sampler2D screen;

out vec4 color;

void main() {
  color = texture(screen, uv);
}