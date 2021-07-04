#version 460

#define NR_POINT_LIGHTS 4

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
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
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {    
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
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalculateDirLight(DirectionalLight aLight, vec3 aNormal, vec3 aViewDirection)
{
    vec3 lightDirection = normalize(-aLight.direction);
    float diffuseShading = max(dot(aNormal, lightDirection), 0.0);
    vec3 reflectDir = reflect(-lightDirection, aNormal);
    float specularShading = pow(max(dot(aViewDirection, reflectDir), 0.0), material.shininess);
    vec3 ambient  = aLight.ambient  * texture(material.diffuse, textureCoordinates).rgb;
    vec3 diffuse  = aLight.diffuse  * diffuseShading * texture(material.diffuse, textureCoordinates).rgb;
    vec3 specular = aLight.specular * specularShading * texture(material.specular, textureCoordinates).rgb;

    return (ambient + diffuse + specular);
}  

vec3 CalculatePointLight(PointLight aLight, vec3 aNormal, vec3 aFragPosition, vec3 aViewDirection)
{
    vec3 lightDirection = normalize(aLight.position - aFragPosition);
    float diffuseShading = max(dot(aNormal, lightDirection), 0.0);
    vec3 reflectionDirection = reflect(-lightDirection, aNormal);
    float specularShading = pow(max(dot(aViewDirection, reflectionDirection), 0.0), material.shininess);
    float lightDistance = length(aLight.position - aFragPosition);
    float attenuation = 1.0 / (aLight.constant + aLight.linear * lightDistance + aLight.quadratic * (lightDistance * lightDistance));    
    vec3 ambient  = aLight.ambient  * texture(material.diffuse, textureCoordinates).rgb;
    vec3 diffuse  = aLight.diffuse  * diffuseShading * texture(material.diffuse, textureCoordinates).rgb;
    vec3 specular = aLight.specular * specularShading * texture(material.specular, textureCoordinates).rgb;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight aLight, vec3 aNormal, vec3 aFragPosition, vec3 aViewDirection)
{
    vec3 lightDirection = normalize(aLight.position - aFragPosition);
    float diffuseShading = max(dot(aNormal, lightDirection), 0.0);
    vec3 reflectionDirection = reflect(-lightDirection, aNormal);
    float specularShading = pow(max(dot(aViewDirection, reflectionDirection), 0.0), material.shininess);
    float Lightdistance = length(aLight.position - aFragPosition);
    float attenuation = 1.0 / (aLight.constant + aLight.linear * Lightdistance + aLight.quadratic * (Lightdistance * Lightdistance));    
    float theta = dot(lightDirection, normalize(-aLight.direction)); 
    float epsilon = aLight.cutOff - aLight.outerCutOff;
    float intensity = clamp((theta - aLight.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = aLight.ambient * texture(material.diffuse, textureCoordinates).rgb;
    vec3 diffuse = aLight.diffuse * diffuseShading * texture(material.diffuse, textureCoordinates).rgb;
    vec3 specular = aLight.specular * specularShading * texture(material.specular, textureCoordinates).rgb;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 normalizedNormal = normalize(normal);
    vec3 viewDirection = normalize(viewPosition - fragPosition);

    vec3 result = CalculateDirLight(directionalLight, normalizedNormal, viewDirection);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += CalculatePointLight(pointLights[i], normalizedNormal, fragPosition, viewDirection);
    }

    result += CalculateSpotLight(spotLight, normalizedNormal, fragPosition, viewDirection);

    fragColor = vec4(result, 1.0);
}
