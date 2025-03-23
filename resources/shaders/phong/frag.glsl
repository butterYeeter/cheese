#version 330 core

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_diffuse2;
  sampler2D texture_diffuse3;
  sampler2D texture_specular1;
  sampler2D texture_specular2;
  sampler2D texture_specular3;
  float shininess;
};

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;

  float constant;
  float linear;
  float quadratic;

  vec3 ambient;
  vec3 specular;
  vec3 diffuse;
};

struct SpotLight {
  vec3 position;
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;

  float cutOff;
  float outerCutOff;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 fragPos;

out vec4 color;

#define NUM_PNT_LIGHTS 4
uniform vec3 viewPos;
uniform Material material;
uniform bool one_channel_specular;
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_PNT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, bool one_channel_specular);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool one_channel_specular);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool t);


void main() {
  float gamma_correction = 1./2.2;
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - fragPos);

  vec3 result = CalcDirLight(dirLight, norm, viewDir, one_channel_specular);


  for (int i = 0; i < NUM_PNT_LIGHTS; i++) {
    result += CalcPointLight(pointLights[i], norm, fragPos, viewDir, one_channel_specular);
  }

  // color = vec4(pow(result, vec3(gamma_correction)), 1.);
  color = vec4(result, 1.);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, bool one_channel_specular) {
  vec3 lightDir = normalize(-light.direction);

  float diff = max(dot(normal, lightDir), 0.);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  vec3 ambient = pow(light.ambient, vec3(1.0)) * vec3(texture(material.texture_diffuse1, TexCoord));
  vec3 diffuse = pow(light.diffuse, vec3(1.0)) * diff * vec3(texture(material.texture_diffuse1, TexCoord));
  vec3 material_specular = (one_channel_specular) ? 
                                                    vec3(texture(material.texture_specular1, TexCoord).r) : 
                                                    vec3(texture(material.texture_specular1, TexCoord));
  vec3 specular = pow(light.specular, vec3(1.0)) * spec * material_specular;

  return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool one_channel_specular) {
  vec3 lightDir = normalize(light.position - fragPos);

  float diff = max(dot(normal, lightDir), 0.);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  float d = length(light.position - fragPos);
  float attenuation = 1/(light.constant + light.linear * d + light.quadratic * d * d);
  
  vec3 ambient = pow(light.ambient, vec3(1.0)) * vec3(texture(material.texture_diffuse1, TexCoord));
  vec3 diffuse = pow(light.diffuse, vec3(1.0)) * diff * vec3(texture(material.texture_diffuse1, TexCoord));
  vec3 material_specular = (one_channel_specular) ? vec3(texture(material.texture_specular1, TexCoord).r) : vec3(texture(material.texture_specular1, TexCoord));
  vec3 specular = pow(light.specular, vec3(1.0)) * spec * material_specular;

  return attenuation * (ambient + diffuse + specular);
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, bool t) {
  vec3 lightDir = normalize(light.position - fragPos);

  float diff = max(dot(normal, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  float d = length(light.position - fragPos);
  float attenuation = 1 / (light.constant + light.linear * d + light.quadratic * d * d);

  vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoord));


  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  return intensity * attenuation * (ambient + diffuse + specular);
}