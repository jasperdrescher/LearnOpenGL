#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aVertexColor;
layout (location = 2) in vec2 aTextureCoordinates;

out vec4 vertexColor;
out vec2 textureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    vertexColor = aVertexColor;
    textureCoordinates = aTextureCoordinates;
}
