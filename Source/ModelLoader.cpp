#include "ModelLoader.h"

#include "FileUtility.h"
#include "LogUtility.h"
#include "Mesh.h"
#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <glm/gtx/hash.hpp>

#include <unordered_map>

ModelLoader::ModelLoader(TextureLoader& aTextureLoader)
    : myTextureLoader(aTextureLoader)
{
}

std::shared_ptr<Model> ModelLoader::LoadModel(const std::string& aFilepath) const
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning;
    std::string error;
    const std::string& fileName = FileUtility::GetNameFromPath(aFilepath);
    const std::string& directory = FileUtility::GetDirectoryFromPath(aFilepath);
    if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, aFilepath.c_str(), directory.c_str(), true))
    {
        LogUtility::PrintError(LogUtility::LogCategory::File, "Failed to load obj %s", fileName.c_str());
        return nullptr;
    }

    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "Loading %s", fileName.c_str());
    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- vertices: %i", attributes.vertices.size());
    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- colors: %i", attributes.colors.size());
    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- normals: %i", attributes.normals.size());
    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- texcoords: %i", attributes.texcoords.size());

    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- shapes: %i", shapes.size());
    for (const tinyobj::shape_t& shape : shapes)
    {
        LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- name: %s", shape.name.c_str());
        LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- indices: %i", shape.mesh.indices.size());
        LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- material_ids: %i", shape.mesh.material_ids.size());
        LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- num_face_vertices: %i", shape.mesh.num_face_vertices.size());
    }

    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- materials: %i", materials.size());

    for (const tinyobj::material_t& material : materials)
        LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- diffuse_texname: %s", material.diffuse_texname.c_str());

    std::shared_ptr<Model> model = std::make_shared<Model>();

    for (const tinyobj::shape_t& shape : shapes)
    {
        Mesh mesh;
        std::unordered_map<glm::vec3, uint32_t> uniqueVertices;
        for (const tinyobj::index_t& index : shape.mesh.indices)
        {
            Vertex vertex;
            size_t vertexIndexStride = 3 * static_cast<size_t>(index.vertex_index);
            vertex.myPosition.x = attributes.vertices[vertexIndexStride];
            vertex.myPosition.y = attributes.vertices[vertexIndexStride + 1];
            vertex.myPosition.z = attributes.vertices[vertexIndexStride + 2];

             if (!attributes.colors.empty())
             {
                 vertex.myColor.x = attributes.colors[vertexIndexStride];
                 vertex.myColor.y = attributes.colors[vertexIndexStride + 1];
                 vertex.myColor.z = attributes.colors[vertexIndexStride + 2];
             }
             else
             {
                vertex.myColor = glm::vec3(1.0f);
             }

            if (uniqueVertices.count(vertex.myPosition) == 0)
            {
                uniqueVertices[vertex.myPosition] = static_cast<uint32_t>(mesh.myVertices.size());
                mesh.myVertices.push_back(vertex);
            }

            mesh.myIndices.push_back(uniqueVertices[vertex.myPosition]);
        }

        model->myMeshes.push_back(mesh);
    }

    std::map<std::string, unsigned int> textures;
    for (const tinyobj::material_t& material : materials)
    {
        if (!material.diffuse_texname.empty())
        {
            if (textures.find(material.diffuse_texname) == textures.end())
            {
                const unsigned int textureIdentifier = myTextureLoader.LoadTexture(directory + material.diffuse_texname);
                textures.insert(std::make_pair(material.diffuse_texname, textureIdentifier));
            }
        }
    }

    for (const std::pair<std::string, unsigned int> pair : textures)
    {
        Texture texture;
        texture.myPath = pair.first;
        texture.myIdentifier = pair.second;
        texture.myType = "texture_diffuse";
        model->myMeshes[0].myTextures.push_back(texture);
    }

    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- textures: %i", textures.size());
    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- meshes: %i", model->myMeshes.size());
    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- indices: %i", model->myMeshes[0].myIndices.size());
    LogUtility::PrintMessage(LogUtility::LogCategory::Graphics, "- vertices: %i", model->myMeshes[0].myVertices.size());

    return model;
}
