#include "TextureLoader.h"

#include "LogUtility.h"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

unsigned int TextureLoader::LoadTexture(const std::string& aFilepath)
{
    const std::size_t hash = std::hash<std::string>{}(aFilepath);

   for (const std::pair<std::size_t, unsigned int> loadedTexture : myLoadedTextures)
   {
       if (loadedTexture.first == hash)
       {
           return loadedTexture.second;
       }
   }

    unsigned int textureID = 0;
    glGenTextures(1, &textureID);

    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = stbi_load(aFilepath.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        LogUtility::PrintError("Texture failed to load at path: %s", aFilepath.c_str());
        stbi_image_free(data);

        return textureID;
    }

    GLenum format = GL_RED;
    switch (channels)
    {
        case 1:
        {
            format = GL_RED;
            break;
        }
        case 3:
        {
            format = GL_RGB;
            break;
        }
        case 4:
        {
            format = GL_RGBA;
            break;
        }
        default:
        {
            LogUtility::PrintError("Failed to map the right number of channels: %i", channels);
            break;
        }
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    myLoadedTextures.emplace(hash, textureID);

    return textureID;
}
