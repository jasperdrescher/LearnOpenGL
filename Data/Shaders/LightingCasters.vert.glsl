#version 460

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoordinates;

out vec3 fragPosition;
out vec3 normal;
out vec2 textureCoordinates;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    fragPosition = vec3(model * vec4(aPosition, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal; 
    textureCoordinates = aTexCoordinates;

    gl_Position = projection * view * vec4(fragPosition, 1.0);
}
