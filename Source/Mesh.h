#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>

class Shader;

struct Vertex
{
    Vertex();

    glm::vec3 myPosition;
    glm::vec3 myColor;
    glm::vec2 myTextureCoordinates;
};

struct Texture
{
    Texture();

    std::string myType;
    std::string myPath;
    unsigned int myIdentifier;
};

class Mesh
{
public:
    Mesh();
    Mesh(std::vector<Vertex> aVertices, std::vector<unsigned int> aIndices, std::vector<Texture> aTextures);

    void SetupMesh();
    void Draw() const;
    void Destroy() const;

    std::vector<Vertex> myVertices;
    std::vector<Texture> myTextures;
    std::vector<unsigned int> myIndices;
    unsigned int myVertexArrayObject;
    unsigned int myVertexBufferObject;

private:
    unsigned int myElementBufferObject;
    unsigned int myVertexAttributePositionIdentifier;
    unsigned int myVertexAttributeColorsIdentifier;
    unsigned int myVertexAttributeTextureCoordinatesIdentifier;
};
