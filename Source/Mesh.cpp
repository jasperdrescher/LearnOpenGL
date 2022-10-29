#include "Mesh.h"

#include <glad/glad.h>

#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"

Mesh::Mesh()
    : myVertexArrayObject(0)
    , myVertexBufferObject(0)
    , myElementBufferObject(0)
    , myVertexAttributePositionIdentifier(0)
    , myVertexAttributeColorsIdentifier(1)
    , myVertexAttributeTextureCoordinatesIdentifier(2)
{
}

Mesh::Mesh(std::vector<Vertex> aVertices, std::vector<unsigned int> aIndices, std::vector<Texture> aTextures)
    : myVertices(std::move(aVertices))
    , myTextures(std::move(aTextures))
    , myIndices(std::move(aIndices))
    , myVertexArrayObject(0)
    , myVertexBufferObject(0)
    , myElementBufferObject(0)
    , myVertexAttributePositionIdentifier(0)
    , myVertexAttributeColorsIdentifier(1)
    , myVertexAttributeTextureCoordinatesIdentifier(2)
{}

void Mesh::Draw() const
{
    if (!myTextures.empty())
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, myTextures[0].myIdentifier);
    }

    glBindVertexArray(myVertexArrayObject);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(myIndices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::Destroy() const
{
    glDeleteVertexArrays(1, &myVertexArrayObject);
    glDeleteBuffers(1, &myVertexBufferObject);
    glDeleteBuffers(1, &myElementBufferObject);
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

    glVertexAttribPointer(myVertexAttributePositionIdentifier, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myPosition)));
    glEnableVertexAttribArray(myVertexAttributePositionIdentifier);

    glVertexAttribPointer(myVertexAttributeColorsIdentifier, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myColor)));
    glEnableVertexAttribArray(myVertexAttributeColorsIdentifier);

    glVertexAttribPointer(myVertexAttributeTextureCoordinatesIdentifier, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myTextureCoordinates)));
    glEnableVertexAttribArray(myVertexAttributeTextureCoordinatesIdentifier);
}
