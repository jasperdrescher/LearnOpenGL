#version 460 core

in vec4 vertexColor;
in vec2 textureCoordinates;

out vec4 fragmentColor;

uniform sampler2D uTexture;

void main()
{
    fragmentColor = texture(uTexture, textureCoordinates) * vertexColor;
}
