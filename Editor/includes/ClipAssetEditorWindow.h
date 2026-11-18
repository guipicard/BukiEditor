#pragma once

#include "AssetEditorWindow.h"
#include "ClipAsset.h"

#include <memory>
#include <string>

namespace buki
{
	struct EditorState;

	class ClipAssetEditorWindow : public AssetEditorWindow
	{
	public:
		ClipAssetEditorWindow(EditorState& editorState, const std::string& assetPath);

		void Draw() override;
		void Reload() override;

	private:
		bool LoadAsset(const std::string& path) override;
		void OnAssetLoaded() override;
		void OnClosed() override;

		ClipAsset m_Asset;
	};
}
