#version 330 core

struct Material {
  sampler2D texture_diffuse1;
};

float near = 0.1;
float far = 100.0;

in vec2 TexCoord;

uniform Material material;

out vec4 fragColor;

float linearizeDepth(float depth);

void main() {
  float depth = linearizeDepth(gl_FragCoord.z) / far;
  // fragColor =  vec4(vec3(depth), 1.0);
  fragColor = texture(material.texture_diffuse1, TexCoord);
}

float linearizeDepth(float depth) {
  float ndc = gl_FragCoord.z * 2.0 - 1.0;
  return (2.0 * near * far) / (far + near - ndc * (far - near));
}