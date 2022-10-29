#pragma once

#include "Vertex.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

template<>
struct std::hash<Vertex>
{
    size_t operator()(const Vertex& aVertex) const noexcept
    {
        return (std::hash<glm::vec3>()(aVertex.myPosition) ^ std::hash<glm::vec2>()(aVertex.myTextureCoordinates) << 1) >> 1;
    }
};
