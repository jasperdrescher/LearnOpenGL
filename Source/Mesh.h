#pragma once

#include "Vertex.h"

#include <vector>

struct Mesh
{
	std::vector<Vertex> myVertices;
	std::vector<unsigned int> myIndices;
};
