#version 330 core

struct Material {
  sampler2D texture_diffuse1;
};

in vec2 TexCoord;

uniform vec3 lightColor;
uniform Material material;

out vec4 fragColor;

void main() {
  fragColor = texture(material.texture_diffuse1, TexCoord) * vec4(pow(lightColor, vec3(1./2.2)), 1.);
}