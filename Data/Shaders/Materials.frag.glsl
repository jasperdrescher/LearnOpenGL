#version 460

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 fragColor;

in vec3 fragPosition;
in vec3 normal;
in vec3 vertexColor;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambientComponent = material.ambient * light.ambient;

    vec3 normalizedNormal = normalize(normal);
    vec3 lightDirection = normalize(lightPosition - fragPosition); 
    float diffuseFactor = max(dot(normalizedNormal, lightDirection), 0.0);
    vec3 diffuseComponent = (diffuseFactor * material.diffuse) * light.diffuse;

    vec3 viewDirection = normalize(viewPosition - fragPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normalizedNormal);
    float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
    vec3 specularComponent = (specularFactor * material.specular) * specularFactor * light.specular;

    vec3 result = (ambientComponent + diffuseComponent + specularComponent) * vertexColor;
    fragColor = vec4(result, 1.0);
}
