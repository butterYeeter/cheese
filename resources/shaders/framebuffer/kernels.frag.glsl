#version 330 core

const float offset = 1.0 / 300.0;

in vec2 uv;

uniform sampler2D screen;

out vec4 color;

void main() {
  vec2 offsets[9] = vec2[](
      vec2(-offset, offset), // top-left
      vec2( 0.0f, offset), // top-center
      vec2( offset, offset), // top-right
      vec2(-offset, 0.0f), // center-left
      vec2( 0.0f, 0.0f), // center-center
      vec2( offset, 0.0f), // center-right
      vec2(-offset, -offset), // bottom-left
      vec2( 0.0f, -offset), // bottom-center
      vec2( offset, -offset) // bottom-right
  );

  float sharpen[9] = float[](
      -1, -1, -1,
      -1, 9, -1,
      -1, -1, -1
  );

  float edge[9] = float[](
      1, 1, 1,
      1, -8, 1,
      1, 1, 1
  );

  float blur[9] = float[](
      1./16, 2./16, 1./16,
      2./16, 4./16, 2./16,
      1./16, 2./16, 1./16
  );

  vec3 sampleTex[9];
  for(int i = 0; i < 9; i++)
  {
    sampleTex[i] = vec3(texture(screen, uv.st +
    offsets[i]));
  }
  vec3 col = vec3(0.0);
  for(int i = 0; i < 9; i++)
    col += sampleTex[i] * edge[i];
  
  color = vec4(col, 1.);
}