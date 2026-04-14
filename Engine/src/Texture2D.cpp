#include "Graphics/Texture2D.h"

#include "Graphics/IGraphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace buki
{
    namespace TextureLoader
    {
        Texture2D LoadFromFile(IGraphics& graphics, const std::string& path, bool flipVertically)
        {
            Texture2D texture{};

            stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);

            int width = 0;
            int height = 0;
            int channels = 0;

            unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
            if (pixels == nullptr)
            {
                return texture;
            }

            texture.id = graphics.CreateTextureRGBA8(pixels, width, height);
            texture.width = width;
            texture.height = height;
            texture.valid = (texture.id != 0);

            stbi_image_free(pixels);
            return texture;
        }

        void Destroy(IGraphics& graphics, Texture2D& texture)
        {
            if (!texture.IsValid())
            {
                return;
            }

            graphics.DestroyTexture(texture.id);

            texture.id = 0;
            texture.width = 0;
            texture.height = 0;
            texture.valid = false;
        }
    }
}
