#pragma once

#include <glm/glm.hpp>

#include <string>

class Shader
{
public:
    Shader();

    void Load(const char* aVertexFilepath, const char* aFragmentFilepath, const char* aGeometryFilepath = nullptr);
    void Use();

    void SetBool(const std::string& aName, const bool aValue) const;
    void SetInt(const std::string& aName, const int aValue) const;
    void SetFloat(const std::string& aName, float aValue) const;
    void SetVec2(const std::string& aName, const glm::vec2& aValue) const;
    void SetVec2(const std::string& aName, float aX, float aY) const;
    void SetVec3(const std::string& aName, const glm::vec3& aValue) const;
    void SetVec3(const std::string& aName, float aX, float aY, float aZ) const;
    void SetVec4(const std::string& aName, const glm::vec4& aValue) const;
    void SetVec4(const std::string& aName, float aX, float aY, float aZ, float aW) const;
    void SetMat2(const std::string& aName, const glm::mat2& aValue) const;
    void SetMat3(const std::string& aName, const glm::mat3& aValue) const;
    void SetMat4(const std::string& aName, const glm::mat4& aValue) const;

    unsigned int myIdentifier;

private:
    void CheckCompileErrors(const unsigned int aShader, const std::string& aType);
    [[nodiscard]] int GetLocation(const std::string& aName) const;
};
