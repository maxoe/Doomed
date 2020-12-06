#include "renderer/texture_loader.h"
#include "renderer/mesh.h"
#include "core/logger.h"

#include <string>

#include <stb_image.h>

GLuint TextureLoader::load(const TextureData& td)
{
    GLuint textureId;

    int width, height, nrComponents;
    unsigned char* data = stbi_load(td.path.c_str(), &width, &height, &nrComponents, 0);

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

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        if (format == GL_RED && (td.type == "textureDiffuse" || td.type == "textureSpecular"))
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return textureId;
    }

    LOG_LOADER_ERROR("Texture failed to load at path: " + td.path);
    stbi_image_free(data);
    return GL_FALSE;
}