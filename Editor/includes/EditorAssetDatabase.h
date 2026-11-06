#pragma once

#include "Ids.h"

#include <string>
#include <vector>
#include <set>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace buki
{
	struct AssetRecord
	{
		EditorId id = InvalidEditorId;
		std::string path;
		std::string assetType;
	};

	class EditorAssetDatabase
	{
	public:
		void ScanDirectory(const std::string& root);

		const AssetRecord* FindByPath(const std::string& path) const;
		const AssetRecord* FindById(EditorId id) const;

		bool RenameAsset(
			const std::string& oldPath,
			const std::string& newPath);

		bool DeleteAsset(const std::string& path);

		bool UpdateReferences(
			const std::string& oldPath,
			const std::string& newPath);
	private:
		std::vector<AssetRecord> m_Assets;
		std::unordered_map<std::string, std::set<std::string>> m_References;

		bool UpdatePrefabReferences(const std::string& prefabPath, const std::string& oldPath, const std::string& newPath);
		bool UpdateAnimationReferences(const std::string& animationPath, const std::string& oldPath, const std::string& newPath);
		void CollectPrefabReferences(const std::string& prefabPath);
		void BuildReferences();


		static EditorId MakeStableId(const std::string& path);
		[[nodiscard]] std::string GetAssetType(const std::string& path) const;
	};
}