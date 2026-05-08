#pragma once
#include "OpenGlTest.h"

#include "Engine.h"
#include "Graphics/IGraphics.h"
#include "Graphics/Texture2D.h"
#include "Units.h"

//#include <Windows.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/constants.hpp>
//#include "ComponentRegistration.h"
//
//REGISTER_COMPONENT(OpenGlTest, "OpenGlTest");

namespace
{
    buki::UVRect MakeUV(const std::array<float, 4>& a)
    {
        return { a[0], a[1], a[2], a[3] };
    }
}

buki::OpenGlTest::OpenGlTest(Entity* entity)
    : MonoBehaviour(entity)
{
}

void buki::OpenGlTest::Start() {}
void buki::OpenGlTest::FixedUpdate(float dt) {}
void buki::OpenGlTest::Destroy() {}
void buki::OpenGlTest::Update(float dt) {}

void buki::OpenGlTest::Draw(float alpha)
{
    m_PlayerTexture = Textures().Load(playerTexturePath);
    if (m_TestTextureId == 0)
    {
        m_TestTextureId = Graphics().CreateTextureRGBA8(kPixels, 2, 2);
    }

    const Camera2D& camera = Engine::Get().GetActiveCamera();

    constexpr float quadSizeMeters = Units::ToMeters(256.0f);
    const float quadSizeMetersRotated = Units::ToMeters(glm::length(glm::vec2(256.0f, 256.0f)));
    constexpr float gapMeters = Units::ToMeters(24.0f);
    const float stepX = quadSizeMetersRotated + gapMeters;
    const float stepY = quadSizeMetersRotated + gapMeters;

    const glm::vec2 topLeftWorld
    {
        camera.position.x - Units::ToMeters(camera.viewportWidth) * 0.5f,
        camera.position.y - Units::ToMeters(camera.viewportHeight) * 0.5f
    };

    glm::vec2 startPos
    {
        topLeftWorld.x + quadSizeMetersRotated * 0.5f + Units::ToMeters(32.0f),
        topLeftWorld.y + quadSizeMetersRotated * 0.5f + Units::ToMeters(32.0f)
    };

    glm::vec2 p = startPos;

    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, quadSizeMeters, quadSizeMeters, glm::radians(45.0f), MakeUV(kFull), m_CyanTint);
    p.x += stepX;
    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, quadSizeMeters, quadSizeMeters, glm::radians(135.0f), MakeUV(kFull), m_MagentaTint);
    p.x += stepX;
    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, quadSizeMeters, quadSizeMeters, glm::radians(225.0f), MakeUV(kFull), m_YellowTint);
    p.x += stepX;                                                                           
    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, quadSizeMeters, quadSizeMeters, glm::radians(315.0f), MakeUV(kFull), m_BlueTint);
    p.x += stepX;                                                                           
    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, Units::ToMeters(256.0f), Units::ToMeters(128.0f), glm::radians(20.0f), MakeUV({ kR[0], kR[1], kG[2], kG[3] }), m_White);

    p = startPos;
    p.y += stepY;

    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, quadSizeMeters, quadSizeMeters, 0.0f, MakeUV(kFull), m_White);
    p.x += stepX;
    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, quadSizeMeters, quadSizeMeters, 0.0f, MakeUV(kR), m_White);
    p.x += stepX;
    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, quadSizeMeters, quadSizeMeters, 0.0f, MakeUV(kG), m_White);
    p.x += stepX;
    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, quadSizeMeters, quadSizeMeters, 0.0f, MakeUV(kB), m_White);
    p.x += stepX;
    Graphics().DrawTexturedQuad(m_TestTextureId, camera, p, quadSizeMeters, quadSizeMeters, 0.0f, MakeUV(kY), m_White);
    p.x += stepX;

    if (m_PlayerTexture != nullptr)
    {
        const RectF playerFrame
        {
            0.0f,
            0.0f,
            static_cast<float>(m_PlayerTexture->width / 4),
            static_cast<float>(m_PlayerTexture->height)
        };

        p.x += stepX;

        Graphics().DrawSprite(
            *m_PlayerTexture,
            camera,
            p,
            Units::ToMeters(playerFrame.w * 5.0f),
            Units::ToMeters(playerFrame.h * 5.0f),
            playerFrame,
            0.0f,
            false,
            false,
            m_White
        );
    }
}

void buki::OpenGlTest::OnCollisionEnter(Entity* other) {}
void buki::OpenGlTest::OnCollisionExit(Entity* other) {}
void buki::OpenGlTest::OnCollisionHit(Entity* other) {}
void buki::OpenGlTest::OnSensorEnter(Entity* other){}
void buki::OpenGlTest::OnSensorExit(Entity* other){}
void buki::OpenGlTest::OnSet() {}
