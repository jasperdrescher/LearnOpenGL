#pragma once

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

	unsigned int GetID() const { return myID; }

private:
	unsigned int myID;
};
