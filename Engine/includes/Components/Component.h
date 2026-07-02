#pragma once
#include "BukiContainers.h"
#include "nlohmann/json.hpp"
#include "PropertyInfo.h"

#include <vector>

using json = nlohmann::json;

namespace buki
{
	class Entity;
	class IInput;
	class ILogger;
	class IGraphics;
	class IAudio;
	class IWorld;
	class PhysicsService;
	class TextureManager;
	class FontManager;
	struct Camera2D;
	 
	class Component
	{
	public:
		virtual ~Component() = default;
		Component();
		Component(Entity* _entity);

		virtual void Start() {}
		virtual void Destroy() {}

		virtual const std::vector<PropertyInfo>& GetProperties() const
		{
			static std::vector<PropertyInfo> empty;
			return empty;
		}

		virtual json Serialize() const;
		virtual void Deserialize(const json& doc);

		virtual void Set() = 0;

		inline Entity* GetEntity() { return m_Entity; }

	protected:
		IInput& Input() const;
		ILogger& Log() const;
		IGraphics& Graphics() const;
		IAudio& Audio() const;
		IWorld& World() const;
		PhysicsService& Physics() const;
		TextureManager& Textures() const;
		FontManager& Fonts() const;
		Camera2D& Camera() const;

		Entity* m_Entity = nullptr;
	};
}
