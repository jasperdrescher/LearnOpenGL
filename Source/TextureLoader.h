#pragma once

#include <string>
#include <map>

class TextureLoader
{
public:
	unsigned int LoadTexture(const std::string& aFilepath);

private:
	std::map<std::size_t, unsigned int> myLoadedTextures;
};
