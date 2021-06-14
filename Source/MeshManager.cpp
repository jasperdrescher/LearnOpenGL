#include "MeshManager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <filesystem>
#include <unordered_map>

Mesh* MeshManager::LoadObj(const std::string& aFilePath)
{
	printf("Loading %s\n", aFilePath.c_str());

	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warning;
	std::string error;

	const std::string& fileName = GetNameFromPath(aFilePath);
	const std::string& directory = GetDirectoryFromPath(aFilePath);
	const std::string& fileExtension = GetExtensionFromPath(aFilePath);

	if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, aFilePath.c_str(), directory.c_str(), true))
	{
		printf("Failed to load %s\n", aFilePath.c_str());
		return nullptr;
	}

	if (!warning.empty())
	{
		printf("%s\n", warning.c_str());
	}

	if (!error.empty())
	{
		printf("%s\n", error.c_str());
	}

	printf("Vertices %i\n", static_cast<int>(attributes.vertices.size()) / 3);
	printf("Normals %i\n", static_cast<int>(attributes.normals.size()) / 3);
	printf("Colors %i\n", static_cast<int>(attributes.colors.size()) / 3);
	printf("Texture coordinates %i\n", static_cast<int>(attributes.texcoords.size()) / 2);
	printf("Materials %i\n", static_cast<int>(materials.size()));
	printf("Shapes %i\n", static_cast<int>(shapes.size()));

	std::unordered_map<Vertex, unsigned int> uniqueVertices;
	Mesh* mesh = new Mesh();

	for (const tinyobj::shape_t& shape : shapes)
	{
		for (const tinyobj::index_t& index : shape.mesh.indices)
		{
			Vertex vertex;
			const size_t vertexIndexStride = 3 * static_cast<size_t>(index.vertex_index);
			vertex.myPosition.x = attributes.vertices[vertexIndexStride + 0];
			vertex.myPosition.y = attributes.vertices[vertexIndexStride + 1];
			vertex.myPosition.z = attributes.vertices[vertexIndexStride + 2];

			if (attributes.normals.size() > 0 )
			{
				const size_t normalIndexStride = 3 * static_cast<size_t>(index.normal_index);
				vertex.myNormal.x = attributes.normals[normalIndexStride + 0];
				vertex.myNormal.y = attributes.normals[normalIndexStride + 1];
				vertex.myNormal.z = attributes.normals[normalIndexStride + 2];
			}

			if (attributes.colors.size() > 0)
			{
				vertex.myColor.x = attributes.colors[vertexIndexStride + 0];
				vertex.myColor.y = attributes.colors[vertexIndexStride + 1];
				vertex.myColor.z = attributes.colors[vertexIndexStride + 2];
			}
			else
			{
				vertex.myColor = glm::vec3(1.0f);
			}

			if (attributes.texcoords.size() > 0)
			{
				const size_t textureCoordinatesIndexStride = 2 * static_cast<size_t>(index.texcoord_index);
				vertex.myTextureCoordinates.x = attributes.texcoords[textureCoordinatesIndexStride + 0];
				vertex.myTextureCoordinates.y = attributes.texcoords[textureCoordinatesIndexStride + 1];
			}

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<unsigned int>(mesh->myVertices.size());
				mesh->myVertices.push_back(vertex);
			}

			mesh->myIndices.push_back(uniqueVertices[vertex]);
		}
	}

	printf("Indices %i\n", static_cast<int>(mesh->myIndices.size()));

	myMeshes.push_back(*mesh);

	return mesh;
}

bool MeshManager::DoesFileExist(const std::string& aPath) const
{
	std::filesystem::file_status fileStatus;
	std::filesystem::path test = std::filesystem::current_path();
	return std::filesystem::status_known(fileStatus) ? std::filesystem::exists(fileStatus) : std::filesystem::exists(aPath);
}

std::string MeshManager::ReadFile(const std::string& aPath) const
{
	std::string sourceResult;
	std::ifstream sourceStream(aPath, std::ios::in);

	if (!sourceStream.is_open())
	{
		printf("File can't be read: %s\n", aPath.c_str());
		return "";
	}

	std::stringstream sourceStringStream;
	sourceStringStream << sourceStream.rdbuf();
	sourceResult = sourceStringStream.str();
	sourceStream.close();

	return sourceResult;
}

std::string MeshManager::GetNameFromPath(const std::string& aPath) const
{
	const std::size_t& nameStartIndex = aPath.find_last_of("/\\") + 1;
	const std::size_t& extensionStartIndex = aPath.find_last_of(".");
	return aPath.substr(nameStartIndex, aPath.length() - nameStartIndex - (aPath.length() - extensionStartIndex));
}

std::string MeshManager::GetExtensionFromPath(const std::string& aPath) const
{
	const std::size_t& extensionStartIndex = aPath.find_last_of(".") + 1;
	return aPath.substr(extensionStartIndex, aPath.length() - extensionStartIndex);
}

std::string MeshManager::GetDirectoryFromPath(const std::string& aPath) const
{
	std::size_t lastSlashIndex = aPath.find_last_of("/\\");
	return aPath.substr(0, lastSlashIndex + 1);
}
