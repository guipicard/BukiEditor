#pragma once
#include "BukiContainers.h"
#include "nlohmann/json.hpp"
#include "PropertyInfo.h"

#include <vector>
#include <set>
#include <string>

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

	enum class AssetReferenceUpdateResult
	{
		None,
		Changed,
		Failed
	};

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

		virtual void CollectAssetReferences(
			std::set<std::string>& outReferences) const;
		virtual AssetReferenceUpdateResult ReplaceAssetReference(const std::string& oldPath, const std::string& newPath);

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
