#include "Mesh.h"
#include "Shader.h"

#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex>& aVertices, const std::vector<unsigned int>& aIndices, const std::vector<Texture>& aTextures)
    : myVertices(aVertices)
    , myTextures(aTextures)
    , myIndices(aIndices)
    , myVertexArrayObject(0)
    , myVertexBufferObject(0)
    , myElementBufferObject(0)
{}

void Mesh::Draw(const Shader& aShader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    for (unsigned int i = 0; i < myTextures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = myTextures[i].myType;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if (name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(aShader.myIdentifier, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, myTextures[i].myIdentifier);
    }

    // draw mesh
    glBindVertexArray(myVertexArrayObject);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(myIndices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &myVertexArrayObject);
    glBindVertexArray(myVertexArrayObject);
    glGenBuffers(1, &myVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, myVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, myVertices.size() * sizeof(Vertex), &myVertices.front(), GL_STATIC_DRAW);

    glGenBuffers(1, &myElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, myIndices.size() * sizeof(unsigned int), &myIndices.front(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myPosition)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myNormal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, myTextureCoordinates)));
    glEnableVertexAttribArray(2);

    // set the vertex attribute pointers
    // vertex Positions
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    //// vertex normals
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, myNormal));
    //// vertex texture coords
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, myTextureCoordinates));
    // vertex tangent
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, myTangent));
    //// vertex bitangent
    //glEnableVertexAttribArray(4);
    //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, myBitangent));
}
