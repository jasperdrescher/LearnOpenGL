#version 460

out vec4 fragColor;

in vec3 fragPosition;
in vec3 normal;
in vec3 vertexColor;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

void main()
{
    float ambientStrength = 0.5;
    vec3 ambientComponent = ambientStrength * lightColor;

    vec3 normalizedNormal = normalize(normal);
    vec3 lightDirection = normalize(lightPosition - fragPosition); 
    float diffuseFactor = max(dot(normalizedNormal, lightDirection), 0.0);
    vec3 diffuseComponent = diffuseFactor * lightColor;

    float specularStrength = 0.5;
    vec3 viewDirection = normalize(viewPosition - fragPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normalizedNormal);
    float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0), 32);
    vec3 specularComponent = specularStrength * specularFactor * lightColor;

    vec3 result = (ambientComponent + diffuseComponent + specularComponent) * vertexColor;
    fragColor = vec4(result, 1.0);
}
