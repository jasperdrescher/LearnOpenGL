#version 460

out vec4 FragColor;
  
in vec4 vertexColor;

void main()
{
    FragColor = vertexColor;
}
