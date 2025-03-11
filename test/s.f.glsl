#version 330 core
in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

uniform sampler2D tex0;

void main() {
  // color = vec4(1.0, 0.0, 0.0, 1.0);
  color = vec4(TexCoord, 0.0, 1.0);
  // color = texture(tex0, TexCoord);
}