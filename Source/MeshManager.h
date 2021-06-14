#pragma once

#include "Mesh.h"

#include <string>

class MeshManager
{
public:
	Mesh* LoadObj(const std::string& aFilePath);

private:
	bool DoesFileExist(const std::string& aPath) const;
	std::string ReadFile(const std::string& aPath) const;
	std::string GetNameFromPath(const std::string& aPath) const;
	std::string GetExtensionFromPath(const std::string& aPath) const;
	std::string GetDirectoryFromPath(const std::string& aPath) const;

	std::vector<Mesh> myMeshes;
};
