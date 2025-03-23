#version 330 core

struct Material {
  sampler2D texture_diffuse1;
};

in vec2 TexCoord;

uniform Material material;

out vec4 fragColor;

void main() {
  fragColor = texture(material.texture_diffuse1, TexCoord);
}