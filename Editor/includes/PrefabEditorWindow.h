#pragma once

#include <string>

#include "AssetEditorWindow.h"
#include "EditorPreviewSessions.h"

namespace buki
{
	struct EditorState;

	class PrefabEditorWindow : public AssetEditorWindow
	{
	public:
		PrefabEditorWindow(EditorState& editorState, const std::string& assetPath);
		void Draw() override;
		void Reload() override;

	private:
		bool LoadAsset(const std::string& path) override;
		void OnAssetLoaded() override;
		void OnClosed() override;

		void UpdateSelection();
		void UpdateCamera();
		void SaveCamera();

		void RenderToFramebuffer();
		void EnsureFramebuffer(int width, int height);
		void DestroyFramebuffer();
		void DrawFramebufferTexture();

		PrefabPreviewSession m_Session;
	};
}