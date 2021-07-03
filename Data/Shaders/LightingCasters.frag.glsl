#version 460

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

out vec4 fragColor;

in vec3 fragPosition;
in vec3 normal;
in vec2 textureCoordinates;

uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambientComponent = light.ambient * texture(material.diffuse, textureCoordinates).rgb;

    vec3 normalizedNormal = normalize(normal);
    vec3 lightDirection = normalize(light.position - fragPosition); 
    float diffuseFactor = max(dot(normalizedNormal, lightDirection), 0.0);
    vec3 diffuseComponent = light.diffuse * diffuseFactor * texture(material.diffuse, textureCoordinates).rgb;

    vec3 viewDirection = normalize(viewPosition - fragPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normalizedNormal);
    float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);
    vec3 specularComponent = light.specular * specularFactor * texture(material.specular, textureCoordinates).rgb;

    float theta = dot(lightDirection, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuseComponent *= intensity;
    specularComponent *= intensity;

    float lightDistance  = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));    
    ambientComponent *= attenuation; 
    diffuseComponent *= attenuation;
    specularComponent *= attenuation;   

    vec3 result = ambientComponent + diffuseComponent + specularComponent;
    fragColor = vec4(result, 1.0);
}
