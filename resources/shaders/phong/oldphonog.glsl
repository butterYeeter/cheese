#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
 };

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 fragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float time;
// uniform float co2;

void main() {
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light.position - FragPos);
  float d = length(light.position - FragPos);
  float attenuation = 1/(light.constant + light.linear * d + light.quadratic * d * d);
  // vec3 lightDir = normalize(-light.direction);

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;
  // vec3 emissive = texture(material.emissive, TexCoord).rgb;
  vec3 emission = vec3(0.0);
  if (texture(material.specular, TexCoord).r == 0.0)   /*rough check for blackbox inside spec texture */
  {
      /*apply emission texture */
      emission = texture(material.emission, TexCoord).rgb;
      /*some extra fun stuff with "time uniform" */
      // emission = texture(material.emission, TexCoord + vec2(0.0,time)).rgb;   /*moving */
      // emission = emission * (sin(time) * 0.5 + 0.5) * 2.0;                     /*fading */
  }
  diffuse *= intensity;
  specular *= intensity;
  vec3 result = attenuation * (ambient + diffuse + specular + emission);
  fragColor = vec4(result, 1.);
}