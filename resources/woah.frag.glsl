#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
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
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_PNT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main() {
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - fragPos);

  vec3 result = CalcDirLight(dirLight, norm, viewDir);


  for (int i = 0; i < NUM_PNT_LIGHTS; i++) {
    result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
  }

  color = vec4(result, 1.);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);

  float diff = max(dot(normal, lightDir), 0.);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

  return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);

  float diff = max(dot(normal, lightDir), 0.);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  float d = length(light.position - fragPos);
  float attenuation = 1/(light.constant + light.linear * d + light.quadratic * d * d);
  
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

  return attenuation * (ambient + diffuse + specular);
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);

  float diff = max(dot(normal, lightDir), 0.0);

  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  float d = length(light.position - fragPos);
  float attenuation = 1 / (light.constant + light.linear * d + light.quadratic * d * d);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));


  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  return intensity * attenuation * (ambient + diffuse + specular);
}