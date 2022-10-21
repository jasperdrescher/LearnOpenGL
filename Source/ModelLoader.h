#pragma once

#include <memory>
#include <string>
#include <vector>

#include "TextureLoader.h"

struct Model;

class ModelLoader
{
public:
	ModelLoader(TextureLoader& aTextureLoader);

	[[nodiscard]] std::shared_ptr<Model> LoadModel(const std::string& aFilepath) const;

private:
	std::vector<std::shared_ptr<Model>> myModels;
	TextureLoader& myTextureLoader;
};
