#version 460 core

out vec4 fragmentColor;

struct Material
{
    sampler2D myDiffuse;
    sampler2D mySpecular;
    float myShininess;
};

struct DirectionalLight
{
    vec3 myDirection;

    vec3 myAmbient;
    vec3 myDiffuse;
    vec3 mySpecular;
};

struct PointLight
{
    vec3 myPosition;

    float myConstant;
    float myLinear;
    float myQuadratic;

    vec3 myAmbient;
    vec3 myDiffuse;
    vec3 mySpecular;
};

struct SpotLight
{
    vec3 myPosition;
    vec3 myDirection;
    float myCutOff;
    float myOuterCutOff;

    float myConstant;
    float myLinear;
    float myQuadratic;

    vec3 myAmbient;
    vec3 myDiffuse;
    vec3 mySpecular;
};

in vec3 fragmentPosition;
in vec3 normal;
in vec2 textureCoordinates;

uniform vec3 viewPosition;
uniform Material material;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[4];
uniform SpotLight spotLight;

vec3 CalculateDirectionalLight(DirectionalLight aLight, vec3 aNormal, vec3 aViewDirection);
vec3 CalculatePointLight(PointLight aLight, vec3 aNormal, vec3 aFragmentPosition, vec3 aViewDirection);
vec3 CalculateSpotLight(SpotLight aLight, vec3 aNormal, vec3 aFragmentPosition, vec3 aViewDirection);

void main()
{
    vec3 normalizedNormal = normalize(normal);
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    vec3 result = CalculateDirectionalLight(directionalLight, normalizedNormal, viewDirection);
    for (int i = 0; i < 4; i++)
        result += CalculatePointLight(pointLights[i], normalizedNormal, fragmentPosition, viewDirection);
    result += CalculateSpotLight(spotLight, normalizedNormal, fragmentPosition, viewDirection);

    fragmentColor = vec4(result, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight aLight, vec3 aNormal, vec3 aViewDirection)
{
    vec3 lightDirection = normalize(-aLight.myDirection);
    float diffuse = max(dot(aNormal, lightDirection), 0.0);
    vec3 reflectionDirection = reflect(-lightDirection, aNormal);
    float specular = pow(max(dot(aViewDirection, reflectionDirection), 0.0), material.myShininess);
    vec3 ambientResult = aLight.myAmbient * vec3(texture(material.myDiffuse, textureCoordinates));
    vec3 diffuseResult = aLight.myDiffuse * diffuse * vec3(texture(material.myDiffuse, textureCoordinates));
    vec3 specularResult = aLight.mySpecular * specular * vec3(texture(material.mySpecular, textureCoordinates));

    return (ambientResult + diffuseResult + specularResult);
}

vec3 CalculatePointLight(PointLight aLight, vec3 aNormal, vec3 aFragmentPosition, vec3 aViewDirection)
{
    vec3 lightDirection = normalize(aLight.myPosition - aFragmentPosition);
    float diffuse = max(dot(aNormal, lightDirection), 0.0);
    vec3 reflectionDirection = reflect(-lightDirection, aNormal);
    float specular = pow(max(dot(aViewDirection, reflectionDirection), 0.0), material.myShininess);
    float distance = length(aLight.myPosition - aFragmentPosition);
    float attenuation = 1.0 / (aLight.myConstant + aLight.myLinear * distance + aLight.myQuadratic * (distance * distance));
    vec3 ambientResult = aLight.myAmbient * vec3(texture(material.myDiffuse, textureCoordinates));
    vec3 diffuseResult = aLight.myDiffuse * diffuse * vec3(texture(material.myDiffuse, textureCoordinates));
    vec3 specularResult = aLight.mySpecular * specular * vec3(texture(material.mySpecular, textureCoordinates));
    ambientResult *= attenuation;
    diffuseResult *= attenuation;
    specularResult *= attenuation;

    return (ambientResult + diffuseResult + specularResult);
}

vec3 CalculateSpotLight(SpotLight aLight, vec3 aNormal, vec3 aFragmentPosition, vec3 aViewDirection)
{
    vec3 lightDirection = normalize(aLight.myPosition - aFragmentPosition);
    float diffuse = max(dot(aNormal, lightDirection), 0.0);
    vec3 reflectionDirection = reflect(-lightDirection, aNormal);
    float specular = pow(max(dot(aViewDirection, reflectionDirection), 0.0), material.myShininess);
    float distance = length(aLight.myPosition - aFragmentPosition);
    float attenuation = 1.0 / (aLight.myConstant + aLight.myLinear * distance + aLight.myQuadratic * (distance * distance));
    float theta = dot(lightDirection, normalize(-aLight.myDirection));
    float epsilon = aLight.myCutOff - aLight.myOuterCutOff;
    float intensity = clamp((theta - aLight.myOuterCutOff) / epsilon, 0.0, 1.0);
    vec3 ambientResult = aLight.myAmbient * vec3(texture(material.myDiffuse, textureCoordinates));
    vec3 diffuseResult = aLight.myDiffuse * diffuse * vec3(texture(material.myDiffuse, textureCoordinates));
    vec3 specularResult = aLight.mySpecular * specular * vec3(texture(material.mySpecular, textureCoordinates));
    ambientResult *= attenuation * intensity;
    diffuseResult *= attenuation * intensity;
    specularResult *= attenuation * intensity;

    return (ambientResult + diffuseResult + specularResult);
}
