#version 330 core

out vec4 fragmentColor;

struct Material
{
    sampler2D myDiffuse;
    sampler2D mySpecular;    
    float myShininess;
}; 

struct Light
{
    vec3 myPosition;  
    vec3 myDirection;
    float myCutOff;
    float myOuterCutOff;
  
    vec3 myAmbient;
    vec3 myDiffuse;
    vec3 mySpecular;
	
    float myConstant;
    float myLinear;
    float myQuadratic;
};

in vec3 fragmentPosition;  
in vec3 normal;  
in vec2 textureCoordinates;
  
uniform vec3 viewPosition;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.myAmbient * texture(material.myDiffuse, textureCoordinates).rgb;
    
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.myPosition - fragmentPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.myDiffuse * diff * texture(material.myDiffuse, textureCoordinates).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPosition - fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.myShininess);
    vec3 specular = light.mySpecular * spec * texture(material.mySpecular, textureCoordinates).rgb;  
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.myDirection)); 
    float epsilon = (light.myCutOff - light.myOuterCutOff);
    float intensity = clamp((theta - light.myOuterCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;
    
    // attenuation
    float distance = length(light.myPosition - fragmentPosition);
    float attenuation = 1.0 / (light.myConstant + light.myLinear * distance + light.myQuadratic * (distance * distance));    
    ambient *= attenuation; 
    diffuse *= attenuation;
    specular *= attenuation;   
        
    vec3 result = ambient + diffuse + specular;
    fragmentColor = vec4(result, 1.0);
}
