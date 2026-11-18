#pragma once

#include "Ids.h"
#include "AssetType.h"
#include "AssetBehaviorRegistry.h"

#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace buki
{
	struct AssetRecord
	{
		EditorId id = InvalidEditorId;
		std::string path;
		AssetType assetType = AssetType::File;
	};

	class EditorAssetDatabase
	{
	public:
		void ScanDirectory(const std::string& root);

		void SetBehaviorRegistry(AssetBehaviorRegistry* behaviors);

		const AssetRecord* FindByPath(const std::string& path) const;

		const AssetRecord* FindById(EditorId id) const;

		bool RenameAsset(const std::string& oldPath, const std::string& newPath);

		bool DeleteAsset(const std::string& path);

		[[nodiscard]] std::vector<std::string> GetReferencedBy(EditorId assetId) const;

		[[nodiscard]] const std::vector<std::string>& GetLastChangedReferencingAssets() const;

		void AddLastChangedReferencingAsset(const std::string& path);

		void UpdateReference(const std::string& ownerAssetPath, const std::string& oldReferencedPath, const std::string& newReferencedPath);

	private:
		AssetRecord* FindRecordByPath(const std::string& path);

		void RemoveRecord(const std::string& path);

		bool MoveFileOnDisk(const std::string& oldPath, const std::string& newPath);

		bool DeleteFileOnDisk(const std::string& path);

		void BuildReferences();

		static EditorId MakeStableId(const std::string& path);

		std::vector<AssetRecord> m_Assets;

		std::unordered_map<EditorId, std::set<std::string>> m_References;

		std::vector<std::string> m_LastChangedReferencingAssets;

		AssetBehaviorRegistry* m_Behaviors = nullptr;
	};
}