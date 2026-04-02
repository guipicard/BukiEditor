//#pragma once
//#include "Component.h"
//#include "IUpdatable.h"
//#include "Color.h"
//
//namespace buki
//{
//	struct Color;
//
//	struct ImguiComponent : public Component, public IUpdatable
//	{
//		ImguiComponent(Entity* entity) : Component(entity) {}
//		~ImguiComponent() = default;
//
//		virtual void Update(float dt) override;
//
//		virtual void Start() override;
//		virtual void Destroy() override;
//
//		virtual json Serialize() override;
//		virtual void Deserialize(json _doc) override;
//		virtual void Set() override;
//	private:
//		bool active = false;
//		Color imguiColor = Color();
//	};
//}