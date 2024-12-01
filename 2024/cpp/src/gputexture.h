#pragma once

struct SDL_GPUTexture;

struct GpuTexture
{
    [[nodiscard]]
    bool isValid() const
    {
        return m_texture != nullptr && m_width > 0.0f && m_height > 0.0f;
    }

    SDL_GPUTexture* m_texture;
    int m_sdlGpuTextureFormatAsInt;
    float m_width;
    float m_height;
};

