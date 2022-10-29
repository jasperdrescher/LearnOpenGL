#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
    Vertex();

    bool operator==(const Vertex& anOther) const;

    glm::vec3 myPosition;
    glm::vec3 myColor;
    glm::vec2 myTextureCoordinates;
};
