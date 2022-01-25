#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>

class Shader;

struct Vertex
{
    Vertex() : myPosition(0.0f), myNormal(0.0f), myTextureCoordinates(0.0f) {}

    glm::vec3 myPosition;
    glm::vec3 myNormal;
    //glm::vec3 myTangent;
    //glm::vec3 myBitangent;
    glm::vec2 myTextureCoordinates;
};

struct Texture
{
    Texture() : myType("N/A"), myPath("N/A"), myIdentifier(0) {}

    std::string myType;
    std::string myPath;
    unsigned int myIdentifier;
};

class Mesh
{
public:
    Mesh() : myVertexArrayObject(0), myVertexBufferObject(0), myElementBufferObject(0) {}
    Mesh(const std::vector<Vertex>& aVertices, const std::vector<unsigned int>& aIndices, const std::vector<Texture>& aTextures);

    void SetupMesh();
    void Draw(const Shader& aShader);

    std::vector<Vertex> myVertices;
    std::vector<Texture> myTextures;
    std::vector<unsigned int> myIndices;
    unsigned int myVertexArrayObject;
    unsigned int myVertexBufferObject;

private:
    unsigned int myElementBufferObject;
};
