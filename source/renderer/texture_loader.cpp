#include "renderer/texture_loader.h"

#include "renderer/mesh.h"

#include <string>
#include <filesystem>
#include <iostream>

#include <stb_image.h>

void TextureLoader::load(std::vector<TextureData>& textures)
{
    for (TextureData& td : textures)
    {
        std::filesystem::path texturePath(APP_TEXTURE_DIR);
        texturePath = texturePath / td.path;

        GLuint textureId;
        glGenTextures(1, &textureId);

        int width, height, nrComponents;
        unsigned char* data =
            stbi_load(texturePath.string().c_str(), &width, &height, &nrComponents, 0);

        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
            {
                format = GL_RED;
            }
            else if (nrComponents == 3)
            {
                format = GL_RGB;
            }
            else
            {
                format = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, textureId);
            glTexImage2D(
                GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);

            td.path = textureId;
            td.isValid = true;
        }
        else
        {
            std::cout << "Texture failed to load at path: " << texturePath << std::endl;
            stbi_image_free(data);
        }
    }
}
