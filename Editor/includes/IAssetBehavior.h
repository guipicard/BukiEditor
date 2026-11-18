#pragma once
#include <string>
#include <vector>

#include "AssetType.h"
namespace buki
{
	struct EditorState;
	struct BrowserEntry;
	class EditorAssetDatabase;

	class IAssetBehavior
	{
	public:
		virtual ~IAssetBehavior() = default;

		[[nodiscard]] virtual AssetType GetType() const = 0;
		[[nodiscard]] virtual const char* GetPayloadType() const = 0;

		virtual bool Create(
			const std::string& directory,
			const std::string& baseName,
			std::string& outPath) = 0;

		virtual void OnDoubleClick(
			EditorState& state,
			const BrowserEntry& entry) = 0;

		virtual void OnDropped(
			EditorState& state,
			const std::string& assetPath,
			const std::string& targetPath) = 0;

		virtual bool UpdateReferences(
			EditorAssetDatabase& database,
			const std::string& referencingAssetPath,
			const std::string& oldReferencedPath,
			const std::string& newReferencedPath) = 0;

		virtual void CollectReferences(
			EditorAssetDatabase& database,
			const std::string& path,
			std::vector<std::string>& outReferences) = 0;

		virtual void OnReferenceChanged(
			EditorAssetDatabase& database,
			const std::string& ownerAssetPath,
			const std::string& oldReferencedPath,
			const std::string& newReferencedPath) = 0;
	};
}
