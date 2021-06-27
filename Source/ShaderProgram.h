#pragma once

#include <glm/mat4x4.hpp>

#include <string>

class ShaderProgram
{
public:
	ShaderProgram();

	void Create(const std::string& aVertexShaderFilePath, const std::string& aFragmentShaderFilePath);
	void Use();
	void Delete();

	void SetBool(const std::string& aName, const bool aValue);
	void SetInt(const std::string& aName, const int aValue);
	void SetFloat(const std::string& aName, const float aValue);
	void SetMatrix4x4(const std::string& aName, const glm::mat4x4& aValue);
	void SetVector3(const std::string& aName, const glm::vec3& aValue);
	void SetVector4(const std::string& aName, const glm::vec4& aValue);

	unsigned int GetID() const { return myID; }
	unsigned int GetLocation(const std::string& aName) const;

private:
	unsigned int myID;
};
