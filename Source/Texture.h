#pragma once

#include <string>

class Texture
{
public:
	Texture();

	void Create(const std::string& aFilePath);

	unsigned int GetID() const { return myID; }

protected:
private:
	unsigned int myID;
	int myWidth;
	int myHeight;
	int myComponents;
};