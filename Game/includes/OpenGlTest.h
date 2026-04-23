#pragma once

#include "MonoBehaviour.h"
#include "IDrawable.h"

#include <array>
#include <cstdint>

namespace buki
{
    struct Texture2D;

    struct OpenGlTest : public MonoBehaviour, public IDrawable
    {
        OpenGlTest(Entity* entity);
        ~OpenGlTest() = default;

        void Awake() override;
        void Start() override;
        void FixedUpdate(float dt) override;
        void Destroy() override;
        void Update(float dt) override;
        void Draw(float alpha) override;
        void OnCollisionEnter(Entity* other) override;
        void OnCollisionExit(Entity* other) override;
        void OnCollisionHit(Entity* other) override;
        void OnSensorEnter(Entity* other) override;
        void OnSensorExit(Entity* other) override;

        json Serialize() override;
        void Deserialize(json _doc) override;
        void Set() override;

    private:
        static constexpr std::array<float, 4> kFull = { 0.0f, 0.0f, 1.0f, 1.0f };
        static constexpr std::array<float, 4> kR = { 0.0f, 0.0f, 0.5f, 0.5f };
        static constexpr std::array<float, 4> kG = { 0.5f, 0.0f, 1.0f, 0.5f };
        static constexpr std::array<float, 4> kB = { 0.0f, 0.5f, 0.5f, 1.0f };
        static constexpr std::array<float, 4> kY = { 0.5f, 0.5f, 1.0f, 1.0f };

        RectF m_FullRect{};
        RectF m_Frame{};

        std::uint32_t m_TestTextureId = 0;
        Texture2D* m_PlayerTexture = nullptr;

        static constexpr unsigned char kPixels[16] =
        {
            255,   0,   0, 255,
              0, 255,   0, 255,
              0,   0, 255, 255,
            255, 255,   0, 255
        };

        Color m_White{ 1.0f, 1.0f, 1.0f, 1.0f };
        Color m_CyanTint{ 0.5f, 1.0f, 1.0f, 1.0f };
        Color m_MagentaTint{ 1.0f, 0.5f, 1.0f, 1.0f };
        Color m_YellowTint{ 1.0f, 1.0f, 0.5f, 1.0f };
        Color m_BlueTint{ 0.5f, 0.5f, 1.0f, 1.0f };
    };
}
