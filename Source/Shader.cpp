#include "Shader.h"

#include "LogUtility.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <fstream>

Shader::Shader()
	: myIdentifier(0)
{}

void Shader::Load(const char* aVertexFilepath, const char* aFragmentFilepath, const char* aGeometryFilepath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;
	std::ifstream geometryShaderFile;

	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	geometryShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vertexShaderFile.open(aVertexFilepath);
		fragmentShaderFile.open(aFragmentFilepath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		vertexShaderFile.close();
		fragmentShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (aGeometryFilepath != nullptr)
		{
			geometryShaderFile.open(aGeometryFilepath);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure& aException)
	{
		LogUtility::PrintError(LogUtility::LogCategory::GL, aException.what());
	}

	const char* vertexShaderCode = vertexCode.c_str();
	const char* fragmentShaderCode = fragmentCode.c_str();
	unsigned int vertexShaderIdentifier = 0;
	unsigned int fragmentShaderIdentifier = 0;

	vertexShaderIdentifier = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderIdentifier, 1, &vertexShaderCode, nullptr);
	glCompileShader(vertexShaderIdentifier);
	CheckCompileErrors(vertexShaderIdentifier, "VERTEX");

	fragmentShaderIdentifier = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderIdentifier, 1, &fragmentShaderCode, nullptr);
	glCompileShader(fragmentShaderIdentifier);
	CheckCompileErrors(fragmentShaderIdentifier, "FRAGMENT");

	unsigned int geometry = 0;
	if (aGeometryFilepath != nullptr)
	{
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, nullptr);
		glCompileShader(geometry);
		CheckCompileErrors(geometry, "GEOMETRY");
	}

	myIdentifier = glCreateProgram();
	glAttachShader(myIdentifier, vertexShaderIdentifier);
	glAttachShader(myIdentifier, fragmentShaderIdentifier);
	if (aGeometryFilepath != nullptr)
		glAttachShader(myIdentifier, geometry);
	glLinkProgram(myIdentifier);
	CheckCompileErrors(myIdentifier, "PROGRAM");

	glDeleteShader(vertexShaderIdentifier);
	glDeleteShader(fragmentShaderIdentifier);
	if (aGeometryFilepath != nullptr)
		glDeleteShader(geometry);
}

void Shader::Use() const
{
	glUseProgram(myIdentifier);
}

void Shader::SetBool(const std::string& aName, const bool aValue) const
{
	glUniform1i(GetLocation(aName), static_cast<int>(aValue));
}

void Shader::SetInt(const std::string& aName, const int aValue) const
{
	glUniform1i(GetLocation(aName), aValue);
}

void Shader::SetFloat(const std::string& aName, float aValue) const
{
	glUniform1f(GetLocation(aName), aValue);
}

void Shader::SetVec2(const std::string& aName, const glm::vec2& aValue) const
{
	glUniform2fv(GetLocation(aName), 1, &aValue[0]);
}

void Shader::SetVec3(const std::string& aName, const glm::vec3& aValue) const
{
	glUniform3fv(GetLocation(aName), 1, &aValue[0]);
}

void Shader::SetVec4(const std::string& aName, const glm::vec4& aValue) const
{
	glUniform4fv(GetLocation(aName), 1, glm::value_ptr(aValue));
}

void Shader::SetMat2(const std::string& aName, const glm::mat2& aValue) const
{
	glUniformMatrix2fv(GetLocation(aName), 1, GL_FALSE, glm::value_ptr(aValue));
}

void Shader::SetMat3(const std::string& aName, const glm::mat3& aValue) const
{
	glUniformMatrix3fv(GetLocation(aName), 1, GL_FALSE, glm::value_ptr(aValue));
}

void Shader::SetMat4(const std::string& aName, const glm::mat4& aValue) const
{
	glUniformMatrix4fv(GetLocation(aName), 1, GL_FALSE, glm::value_ptr(aValue));
}

void Shader::CheckCompileErrors(const unsigned int aShader, const std::string& aType)
{
	GLint success;
	GLchar infoLog[1024];
	if (aType != "PROGRAM")
	{
		glGetShaderiv(aShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(aShader, 1024, nullptr, infoLog);
			LogUtility::PrintError(LogUtility::LogCategory::GL, "Shader compilation for %s failed because %s", aType.c_str(), infoLog);
		}
	}
	else
	{
		glGetProgramiv(aShader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(aShader, 1024, nullptr, infoLog);
			LogUtility::PrintError(LogUtility::LogCategory::GL, "Shader linking for %s failed because %s", aType.c_str(), infoLog);
		}
	}
}

int Shader::GetLocation(const std::string& aName) const
{
	return glGetUniformLocation(myIdentifier, aName.c_str());
}
