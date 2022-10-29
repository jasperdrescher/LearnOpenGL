#pragma once

#include <vector>

struct Texture;
struct Vertex;
class Shader;

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
