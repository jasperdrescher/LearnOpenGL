#include "Mesh.h"
#include "Shader.h"

#include <glad/glad.h>

Vertex::Vertex()
    : myPosition(0.0f)
    , myColor(0.0f)
{
}

Texture::Texture()
    : myType("N/A")
    , myPath("N/A")
    , myIdentifier(0)
{
}

Mesh::Mesh()
    : myVertexArrayObject(0)
    , myVertexBufferObject(0)
    , myElementBufferObject(0)
{
}

Mesh::Mesh(std::vector<Vertex> aVertices, std::vector<unsigned int> aIndices, std::vector<Texture> aTextures)
    : myVertices(std::move(aVertices))
    , myTextures(std::move(aTextures))
    , myIndices(std::move(aIndices))
    , myVertexArrayObject(0)
    , myVertexBufferObject(0)
    , myElementBufferObject(0)
{}

void Mesh::Draw() const
{
    glBindVertexArray(myVertexArrayObject);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(myIndices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &myVertexArrayObject);
    glBindVertexArray(myVertexArrayObject);
    glGenBuffers(1, &myVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, myVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLintptrARB>(myVertices.size() * sizeof(Vertex)), &myVertices.front(), GL_STATIC_DRAW);

    glGenBuffers(1, &myElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLintptrARB>(myIndices.size() * sizeof(unsigned int)), &myIndices.front(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myPosition)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myColor)));
    glEnableVertexAttribArray(1);
}
