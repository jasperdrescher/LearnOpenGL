#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
	Vertex::Vertex()
		: myPosition(0.0f)
		, myNormal(0.0f)
		, myColor(0.0f)
		, myTextureCoordinates(0.0f)
	{

	}

	glm::vec3 myPosition;
	glm::vec3 myNormal;
	glm::vec3 myColor;
	glm::vec2 myTextureCoordinates;

	bool operator==(const Vertex& aOther) const
	{
		return myPosition == aOther.myPosition && myNormal == aOther.myNormal && myColor == aOther.myColor && myTextureCoordinates == aOther.myTextureCoordinates;
	}
};

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& aVertex) const
		{
			return ((hash<glm::vec3>()(aVertex.myPosition) ^
				(hash<glm::vec3>()(aVertex.myColor) << 1)) >> 1) ^
				(hash<glm::vec2>()(aVertex.myTextureCoordinates) << 1);
		}
	};
}
