#pragma once

#include "IGraphics.h"
#include "Memory.h"

namespace buki
{
    class IPlatform;

    class OpenGLGraphics final : public IGraphics
    {
    public:
        bool Initialize(IPlatform& platform) override;
        void Shutdown() override;

        void BeginFrame() override;
        void EndFrame() override;
        void Resize(int width, int height) override;

    private:
        bool InitializeLoader(IPlatform& platform);
        bool CreateTestTriangle();
        bool CreateShaderProgram();
        void DestroyTriangle();
        void DestroyShaderProgram();

    private:
        IPlatform* m_Platform = nullptr;

        std::uint32_t m_ShaderProgram = 0;
        std::uint32_t m_VAO = 0;
        std::uint32_t m_VBO = 0;

        int m_ViewportWidth = 0;
        int m_ViewportHeight = 0;
    };
}
