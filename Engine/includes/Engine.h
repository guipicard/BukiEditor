#pragma once
#include "Platform/IPlatform.h"

#include "Graphics/TextureManager.h"
#include "Graphics/IGraphics.h"
#include "Core/IInput.h"
#include "Core/ILogger.h"
#include "Core/IWorld.h"
#include "Core/IAudio.h"
#include "PhysicsService.h"
#include <cstdint>
#include <string>
#include "Memory.h"
#include "Graphics/Camera2D.h"

namespace buki {
    using Uint32 = std::uint32_t;

    class Engine final {
    public:
        static Engine& Get()
        {
            static Engine instance;
            return instance;
        }
        bool Init(const std::string& title, int w, int h);
        void Start();

        int GetFps() const { return m_CurrentFPS; }
        int GetPps() const { return m_CurrentPPS; }

        float GetTimeScale() const { return m_TimeScale; }
        void SetTimeScale(float scale);

        IPlatform& Platform() const { return *m_Platform; }
        IGraphics& Graphics() const { return *m_Graphics; }
        IInput& Input() const { return *m_Input; }
        ILogger& Log() const { return *m_Console; }
        IAudio& Audio() const { return *m_Audio; }
        IWorld& World() const { return *m_World; }
        PhysicsService& Physics() const { return *m_Physics; }
        TextureManager& Textures() { return *m_TextureManager; }
        Camera2D& GetActiveCamera() { return m_Camera; }
        const Camera2D& GetActiveCamera() const { return m_Camera; }
    private:
        Engine() = default;

        void ProcessInput();
        void FixedUpdate(const float dt);
        void Update(float dt);
        void Render(float alpha);
        void Shutdown();

        bool m_IsInit = false;
        static constexpr Uint32 TARGET_FPS = 160;
        static constexpr Uint32 MS_PER_FRAME = 1000 / TARGET_FPS;
        static constexpr Uint32 TARGET_PPS = 60;
        static constexpr Uint32 FIXED_TIMESTEP = 1000 / TARGET_PPS;

        int m_CurrentFPS = 0;
        int m_CurrentPPS = 0;
        float m_TimeScale = 1.0f;

        ILogger* m_Console = nullptr;
        IPlatform* m_Platform = nullptr;
        IGraphics* m_Graphics = nullptr;
        IInput* m_Input = nullptr;
        IWorld* m_World = nullptr;
        PhysicsService* m_Physics = nullptr;
        Camera2D m_Camera;
        IAudio* m_Audio = nullptr;
        //Editor* m_Editor = nullptr;

        Scope<TextureManager> m_TextureManager;
    };
}