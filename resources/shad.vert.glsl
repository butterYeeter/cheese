#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float time;
const float pi = 3.14159265;

void main() {
  TexCoord = aTexCoord;
  fragPos = vec3(model * vec4(aPos, 1.));
  Normal = mat3(transpose(inverse(model))) * aNormal;


  float len = length(aPos);
  // vec3 pos = aPos + vec3(sin(5.00 * pi * len + time * 5) * 0.03 + 0.015);
  gl_Position = proj * view * model * vec4(aPos, 1.);
}