#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace buki
{
	class IAssetBehavior;
	enum class AssetType;

	class AssetBehaviorRegistry
	{
	public:
		void Register(AssetType type, std::unique_ptr<IAssetBehavior> behavior);

		[[nodiscard]] IAssetBehavior* Find(AssetType type) const;

		[[nodiscard]] IAssetBehavior* FindByPath(const std::string& path) const;

		AssetType GetAssetType(const std::string& path) const;

	private:
		std::unordered_map<AssetType, std::unique_ptr<IAssetBehavior>> m_Behaviors;
	};
}