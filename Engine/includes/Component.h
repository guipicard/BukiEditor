#pragma once
#include "BukiContainers.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace buki
{
	struct Entity;
	struct IInput;
	struct ILogger;
	struct IGraphics;
	//struct IAudio;
	struct ICollision;
	struct IWorld;
	struct PhysicsService;
	struct TextureManager;

	struct Component
	{
	public:
		virtual ~Component() = default;
		Component();
		Component(Entity* _entity);

		virtual void Start() {}
		virtual void Destroy() {}

		virtual json Serialize() = 0;
		virtual void Deserialize(json _doc) = 0;
		virtual void Set() = 0;

		inline Entity* GetEntity() { return m_Entity; }

		inline bool IsSerialized() const { return m_Serialized; }
		inline bool IsDeserialized() const { return m_Deserialized; }
		inline void SetSerialized(const bool state) { m_Serialized = state; }
		inline void SetDeserialized(const bool state) { m_Deserialized = state; }
		inline bool IsSavable() const { return m_Savable; }

	protected:
		IInput& Input() const;
		ILogger& Log() const;
		IGraphics& Graphics() const;
		//IAudio& Audio() const;
		IWorld& World() const;
		PhysicsService& Physics() const;
		TextureManager& Textures() const;

		Entity* m_Entity = nullptr;
		bool m_Serialized = false;
		bool m_Deserialized = false;
		bool m_Savable = true;
	};
}
