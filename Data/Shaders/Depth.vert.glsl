#version 460

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoordinates;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec3 fragPosition = vec3(model * vec4(aPosition, 1.0));

    gl_Position = projection * view * vec4(fragPosition, 1.0);
}
