#version 460

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 fragPosition;
out vec3 normal;
out vec3 vertexColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vertexColor = aColor;
    fragPosition = vec3(model * vec4(aPosition, 1.0));
    normal = vec3(view * model * vec4(aNormal, 1.0));

    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}