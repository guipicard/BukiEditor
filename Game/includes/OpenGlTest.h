#pragma once
#include "MonoBehaviour.h"
#include "IDrawable.h"

namespace buki
{
	struct Sprite;
	struct RigidBody;
	struct Animation;
	struct Box;
	struct PlayerState;
	struct Text;
	struct Texture2D;
	struct OpenGlTest : public MonoBehaviour, public IDrawable
	{
		OpenGlTest(Entity* entity);
		~OpenGlTest() = default;
		virtual void Awake() override;
		virtual void Start() override;
		virtual void FixedUpdate(const float dt) override;
		virtual void Destroy() override;
		virtual void Update(const float dt) override;
		virtual void Draw(const float alpha) override;
		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;

		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;
	private:
		std::array<float, 4> full = { 0.0f, 0.0f, 1.0f, 1.0f };
		std::array<float, 4> r = { 0.0f, 0.0f, 0.5f, 0.5f };
		std::array<float, 4> g = { 0.5f, 0.0f, 1.0f, 0.5f };
		std::array<float, 4> b = { 0.0f, 0.5f, 0.5f, 1.0f };
		std::array<float, 4> y = { 0.5f, 0.5f, 1.0f, 1.0f };
		std::array<float, 4> uv = full;

		RectF fullRect;
		RectF frame;
		
		std::uint32_t tex;
		Texture2D* playerTexture;
	};
}
