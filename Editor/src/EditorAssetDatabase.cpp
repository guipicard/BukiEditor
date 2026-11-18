#include "EditorAssetDatabase.h"
#include "EditorAssetEntries.h"
#include "Engine.h"
#include "IAssetBehavior.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace buki
{
	namespace
	{
		std::string CanonicalizePath(
			const std::string& path)
		{
			return ToAssetRelativePath(fs::absolute(path).lexically_normal());
		}
	}

	void EditorAssetDatabase::SetBehaviorRegistry(
		AssetBehaviorRegistry* behaviors)
	{
		m_Behaviors = behaviors;
	}

	void EditorAssetDatabase::ScanDirectory(
		const std::string& root)
	{
		m_Assets.clear();

		const fs::path rootPath =
			fs::absolute(root).lexically_normal();

		if (!fs::exists(rootPath) ||
			!fs::is_directory(rootPath))
		{
			return;
		}

		for (const fs::directory_entry& entry :
			fs::recursive_directory_iterator(rootPath))
		{
			if (!entry.is_regular_file())
				continue;

			const std::string canonicalPath = ToAssetRelativePath(fs::absolute(entry.path()).lexically_normal());

			AssetRecord record;
			record.path = canonicalPath;

			if (m_Behaviors)
			{
				record.assetType =
					m_Behaviors->GetAssetType(canonicalPath);
			}
			else
			{
				record.assetType = AssetType::File;
			}

			record.id = MakeStableId(canonicalPath);

			m_Assets.push_back(record);
		}

		std::sort(
			m_Assets.begin(),
			m_Assets.end(),
			[](const AssetRecord& a, const AssetRecord& b)
			{
				return a.path < b.path;
			});

		BuildReferences();
	}

	const AssetRecord* EditorAssetDatabase::FindByPath(const std::string& path) const
	{
		const std::string canonicalPath =
			CanonicalizePath(path);

		for (const AssetRecord& record : m_Assets)
		{
			if (record.path == canonicalPath)
			{
				return &record;
			}
		}

		return nullptr;
	}

	const AssetRecord* EditorAssetDatabase::FindById(
		EditorId id) const
	{
		for (const AssetRecord& record : m_Assets)
		{
			if (record.id == id)
			{
				return &record;
			}
		}

		return nullptr;
	}

	AssetRecord* EditorAssetDatabase::FindRecordByPath(
		const std::string& path)
	{
		const std::string canonicalPath =
			CanonicalizePath(path);

		for (AssetRecord& record : m_Assets)
		{
			if (record.path == canonicalPath)
			{
				return &record;
			}
		}

		return nullptr;
	}

	void EditorAssetDatabase::RemoveRecord(
		const std::string& path)
	{
		const std::string canonicalPath =
			CanonicalizePath(path);

		std::erase_if(
			m_Assets,
			[&canonicalPath](const AssetRecord& record)
			{
				return record.path == canonicalPath;
			});
	}

	bool EditorAssetDatabase::MoveFileOnDisk(
		const std::string& oldPath,
		const std::string& newPath)
	{
		const fs::path oldFilePath =
			ToAbsolutePath(oldPath);

		const fs::path newFilePath =
			ToAbsolutePath(newPath);

		std::error_code error;

		fs::rename(oldFilePath, newFilePath, error);

		if (error)
		{
			Engine::Get().Log().LogError(
				"Failed to rename asset: " +
				error.message());

			return false;
		}

		return true;
	}

	bool EditorAssetDatabase::DeleteFileOnDisk(
		const std::string& path)
	{
		const fs::path filePath =
			ToAbsolutePath(path);

		std::error_code error;

		fs::remove(filePath, error);

		if (error)
		{
			Engine::Get().Log().LogError(
				"Failed to delete asset: " +
				error.message());

			return false;
		}

		return true;
	}

	bool EditorAssetDatabase::RenameAsset(
		const std::string& oldPath,
		const std::string& newPath)
	{
		AssetRecord* record =
			FindRecordByPath(oldPath);

		if (!record)
		{
			return false;
		}

		const std::string canonicalOldPath =
			CanonicalizePath(oldPath);

		const std::string canonicalNewPath =
			CanonicalizePath(newPath);

		const EditorId oldAssetId =
			record->id;

		m_LastChangedReferencingAssets.clear();

		const auto referencesIt =
			m_References.find(oldAssetId);

		if (referencesIt != m_References.end())
		{
			for (const std::string& referencingPath : referencesIt->second)
			{
				IAssetBehavior* behavior =
					m_Behaviors->FindByPath(referencingPath);

				if (!behavior)
					continue;

				behavior->UpdateReferences(
					*this,
					referencingPath,
					canonicalOldPath,
					canonicalNewPath);
			}
		}

		if (!MoveFileOnDisk(
			canonicalOldPath,
			canonicalNewPath))
		{
			return false;
		}

		record->path = canonicalNewPath;
		record->id = MakeStableId(canonicalNewPath);

		BuildReferences();

		return true;
	}

	bool EditorAssetDatabase::DeleteAsset(
		const std::string& path)
	{
		AssetRecord* record =
			FindRecordByPath(path);

		if (!record)
		{
			return false;
		}

		const std::string canonicalPath =
			CanonicalizePath(path);

		const EditorId assetId =
			record->id;

		m_LastChangedReferencingAssets.clear();

		const auto referencesIt =
			m_References.find(assetId);

		if (referencesIt != m_References.end())
		{
			for (const std::string& referencingPath : referencesIt->second)
			{
				IAssetBehavior* behavior =
					m_Behaviors->FindByPath(referencingPath);

				if (!behavior)
					continue;

				behavior->UpdateReferences(
					*this,
					referencingPath,
					canonicalPath,
					"");
			}
		}

		if (!DeleteFileOnDisk(canonicalPath))
		{
			return false;
		}

		RemoveRecord(canonicalPath);

		BuildReferences();

		return true;
	}

	std::vector<std::string>
		EditorAssetDatabase::GetReferencedBy(
			EditorId assetId) const
	{
		std::vector<std::string> result;

		const auto it = m_References.find(assetId);

		if (it == m_References.end())
		{
			return result;
		}

		result.assign(
			it->second.begin(),
			it->second.end());

		return result;
	}

	const std::vector<std::string>&
		EditorAssetDatabase::GetLastChangedReferencingAssets() const
	{
		return m_LastChangedReferencingAssets;
	}

	void EditorAssetDatabase::AddLastChangedReferencingAsset(
		const std::string& path)
	{
		m_LastChangedReferencingAssets.push_back(
			CanonicalizePath(path));
	}

	void EditorAssetDatabase::UpdateReference(const std::string& ownerAssetPath, const std::string& oldReferencedPath, const std::string& newReferencedPath)
	{
		const std::string ownerPath = CanonicalizePath(ownerAssetPath);

		const EditorId ownerId =MakeStableId(ownerPath);

		if (!oldReferencedPath.empty())
		{
			const AssetRecord* oldRecord =FindByPath(oldReferencedPath);

			if (oldRecord)
			{
				auto it = m_References.find(oldRecord->id);

				if (it != m_References.end())
				{
					it->second.erase(ownerPath);

					if (it->second.empty())
					{
						m_References.erase(it);
					}
				}
			}
		}

		if (!newReferencedPath.empty())
		{
			const AssetRecord* newRecord = FindByPath(newReferencedPath);

			if (newRecord)
			{
				m_References[newRecord->id].insert(ownerPath);
			}
		}
	}

	void EditorAssetDatabase::BuildReferences()
	{
		m_References.clear();

		for (const AssetRecord& record : m_Assets)
		{
			if (!m_Behaviors)
				continue;

			IAssetBehavior* behavior =
				m_Behaviors->FindByPath(record.path);

			if (!behavior)
				continue;

			std::vector<std::string> references;

			behavior->CollectReferences(
				*this,
				record.path,
				references);

			for (const std::string& referencedPath : references)
			{
				if (referencedPath.empty())
					continue;

				const AssetRecord* referencedRecord =
					FindByPath(referencedPath);

				if (!referencedRecord)
					continue;

				m_References[referencedRecord->id].insert(
					record.path);
			}
		}
	}

	EditorId EditorAssetDatabase::MakeStableId(
		const std::string& path)
	{
		std::uint64_t hash = 1469598103934665603ULL;

		for (const char character : path)
		{
			hash ^= static_cast<std::uint64_t>(
				static_cast<unsigned char>(character));

			hash *= 1099511628211ULL;
		}

		return hash == InvalidEditorId ? 1 : hash;
	}
}