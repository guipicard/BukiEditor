#pragma once
#include "MonoBehaviour.h"

namespace buki
{
	struct Text;
	struct EditorEntity;

	struct EditorController : public MonoBehaviour
	{
		EditorController(Entity* entity);
		virtual void Awake() override;
		virtual void Start() override;
		virtual void FixedUpdate(const float dt) override;
		virtual void Destroy() override;
		virtual void Update(const float dt) override;
		virtual void OnCollisionEnter(Entity* other) override;
		virtual void OnCollisionExit(Entity* other) override;
		virtual void OnCollisionHit(Entity* other) override;

		void SetCanStopTime(bool canStop) { canStopTime = canStop; }

		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;
	private:
		void SetTextPosition();
		bool movingScreen = false;
		Vector2 bgOffset;
		Vector2 firstMousePos;
		bool canStopTime = true;
		Text* fpsText;
		EditorEntity* selectedEntity;
	};
}