#pragma once
#include "AssetBehaviors.h"

#include "ClipAsset.h"
#include "Entity.h"
#include "EditorAssetEntries.h"
#include "EditorAssetDatabase.h"
#include "EditorState.h"
#include "AnimationStateMachineEditorWindow.h"
#include "PrefabEditorWindow.h"
#include "ClipAssetEditorWindow.h"
#include "Iscene.h"
#include <filesystem>
#include <fstream>

namespace buki
{
	void AssetBehaviorBase::OnReferenceChanged(EditorAssetDatabase& database, const std::string& ownerAssetPath, const std::string& oldReferencedPath, const std::string& newReferencedPath)
	{

		database.UpdateReference(
			ownerAssetPath,
			oldReferencedPath,
			newReferencedPath);

	}

	bool AssetBehaviorBase::UpdateReferences(EditorAssetDatabase& database, const std::string& referencingAssetPath, const std::string& oldReferencedPath, const std::string& newReferencedPath)
	{
		// Most asset types do not reference other assets yet.
		return true;
	}

	bool ClipAssetBehavior::Create(
		const std::string& directory,
		const std::string& baseName,
		std::string& outPath)
	{
		const std::string name = baseName.empty()
			? "NewClip"
			: baseName;

		std::string path =
			directory + "/" + name + ".clip";

		int suffix = 1;

		while (std::ifstream(path).good())
		{
			path = directory + "/" + name +
				std::to_string(suffix++) + ".clip";
		}

		ClipAsset clip;
		clip.name = name;
		clip.path = path;
		clip.loop = true;

		SpriteFrame frame;
		frame.texturePath = "";
		frame.duration = 0.1f;

		clip.frames.push_back(frame);

		if (!clip.SaveToFile(path))
			return false;

		outPath = path;
		return true;
	}

	void ClipAssetBehavior::OnDoubleClick(
		EditorState& state,
		const BrowserEntry& entry)
	{
		state.assetEditorWindows.OpenAssetWindow<ClipAssetEditorWindow>(
			entry.path,
			state,
			entry.path);
	}

	void ClipAssetBehavior::OnDropped(
		EditorState& state,
		const std::string& assetPath,
		const std::string& targetPath)
	{
		// Clips are not instantiated into scenes,
		// so this can remain empty for now.
	}

	bool ClipAssetBehavior::UpdateReferences(
		EditorAssetDatabase& database,
		const std::string& referencingAssetPath,
		const std::string& oldReferencedPath,
		const std::string& newReferencedPath)
	{
		return true;
	}

	void ClipAssetBehavior::CollectReferences(EditorAssetDatabase& database, const std::string& path, std::vector<std::string>& outReferences)
	{
	}

	bool AnimationAssetBehavior::Create(
		const std::string& directory,
		const std::string& baseName,
		std::string& outPath)
	{
		const std::string name = baseName.empty()
			? "NewAnimation"
			: baseName;

		std::string path =
			directory + "/" + name + ".anim";

		int suffix = 1;

		while (std::ifstream(path).good())
		{
			path = directory + "/" + name +
				std::to_string(suffix++) + ".anim";
		}

		AnimationAsset asset;
		asset.name = name;
		asset.path = path;
		asset.initialState = "Idle";

		AnimationState idleState;
		idleState.id = GenerateEditorId();
		idleState.name = "Idle";
		idleState.clipPath = "";

		asset.states.push_back(idleState);

		if (!asset.SaveToFile(path))
			return false;

		outPath = path;
		return true;
	}

	void AnimationAssetBehavior::OnDoubleClick(
		EditorState& state,
		const BrowserEntry& entry)
	{
		state.assetEditorWindows.OpenAssetWindow<
			AnimationStateMachineEditorWindow>(
				entry.path,
				state,
				entry.path);
	}

	void AnimationAssetBehavior::OnDropped(
		EditorState& state,
		const std::string& assetPath,
		const std::string& targetPath)
	{
		// Animations are not dropped into scenes yet.
	}

	bool AnimationAssetBehavior::UpdateReferences(
		EditorAssetDatabase& database,
		const std::string& referencingAssetPath,
		const std::string& oldReferencedPath,
		const std::string& newReferencedPath)
	{
		AnimationAsset asset;

		if (!asset.LoadFromFile(referencingAssetPath))
			return false;

		bool changed = false;

		for (AnimationState& state : asset.states)
		{
			if (state.clipPath == oldReferencedPath)
			{
				state.clipPath = newReferencedPath;
				changed = true;
			}
		}

		if (!changed)
			return true;

		if (!asset.SaveToFile(referencingAssetPath))
			return false;

		database.AddLastChangedReferencingAsset(
			referencingAssetPath);

		return true;
	}

	void AnimationAssetBehavior::CollectReferences(
		EditorAssetDatabase& database,
		const std::string& path,
		std::vector<std::string>& outReferences)
	{
		AnimationAsset asset;

		if (!asset.LoadFromFile(path))
			return;

		for (const AnimationState& state : asset.states)
		{
			if (state.clipPath.empty())
				continue;

			outReferences.push_back(state.clipPath);
		}
	}

	bool PrefabAssetBehavior::Create(
		const std::string& directory,
		const std::string& baseName,
		std::string& outPath)
	{
		// Implement when you add prefab creation to the browser.
		return false;
	}

	void buki::PrefabAssetBehavior::OnDoubleClick(EditorState& state, const BrowserEntry& entry)
	{
		state.assetEditorWindows.OpenAssetWindow<
			PrefabEditorWindow>(
				entry.path,
				state,
				entry.path);
	}

	void PrefabAssetBehavior::OnDropped(
		EditorState& state,
		const std::string& assetPath,
		const std::string& targetPath)
	{
		auto* world = Engine::Get().GetWorldPtr();

		if (world)
		{
			world->InstantiatePrefab(assetPath);
		}
	}

	bool PrefabAssetBehavior::UpdateReferences(EditorAssetDatabase& database, const std::string& referencingAssetPath, const std::string& oldReferencedPath, const std::string& newReferencedPath)
	{
		// Add this when prefabs can reference other prefabs/assets.
		return true;
	}
	void PrefabAssetBehavior::CollectReferences(EditorAssetDatabase& database, const std::string& path, std::vector<std::string>& outReferences)
	{
	}
	void buki::SceneAssetBehavior::OnDoubleClick(EditorState& state,const BrowserEntry& entry)
	{
		auto* world = Engine::Get().GetWorldPtr();

		if (!world)
		{
			return;
		}

		const std::string scenePath =ToAssetRelativePath(entry.path);

		if (scenePath.empty())
		{
			return;
		}

		// Temporary safe policy until a Save / Discard / Cancel dialog exists.
		if (state.IsSceneDirty())
		{
			Engine::Get().Log().LogWarning("Save the current scene before opening another scene.");
			return;
		}

		// Save the outgoing scene's editor camera settings.
		if (!state.scenePreviewSession.path.empty())
		{
			state.scenePreviewSession.cameraSettings.Serialize(
				state.scenePreviewSession.path);
		}

		// LoadScene() can destroy the current world before returning false.
		state.selectedEntity = nullptr;
		state.activeEntity = nullptr;
		state.selectedEntities.clear();

		state.selectedComponent = nullptr;
		state.activeComponent = nullptr;
		state.selectedComponents.clear();

		state.selectedPrefabEntity = nullptr;
		state.selectedPrefabPath.clear();

		state.selectedScenePath.clear();
		state.scenePreviewSession.path.clear();
		state.scenePreviewSession.prefabEntities.clear();

		if (!world->LoadScene(scenePath))
		{
			state.ClearSceneDirty();

			Engine::Get().Log().LogError(
				"Failed to load scene: " + scenePath);

			return;
		}

		state.selectedScenePath = scenePath;
		state.scenePreviewSession.path = scenePath;

		auto& settings =
			state.scenePreviewSession.cameraSettings;

		settings = CameraSettings{};

		// Start with the camera loaded from the scene file.
		if (Camera2D* camera =
			Engine::Get().GetActiveCameraPtr())
		{
			settings.position =
			{
				camera->position.x,
				camera->position.y
			};

			settings.zoom = camera->zoom;

			// Saved editor settings override the scene-file defaults.
			settings.Deserialize(scenePath);

			camera->position =
			{
				settings.position.x,
				settings.position.y
			};

			camera->zoom = settings.zoom;
		}

		state.viewportDisplayMode = settings.displayMode;

		state.scenePreviewSession.open = true;
		state.scenePreviewSession.focused = false;
		state.scenePreviewSession.hovered = false;

		state.showSceneView = true;
		state.requestSceneWindowFocus = true;

		state.ClearSceneDirty();
	}
	void SceneAssetBehavior::OnDropped(EditorState& state, const std::string& assetPath, const std::string& targetPath)
	{
	}
	bool SceneAssetBehavior::UpdateReferences(EditorAssetDatabase& database, const std::string& referencingAssetPath, const std::string& oldReferencedPath, const std::string& newReferencedPath)
	{
		return false;
	}
	void SceneAssetBehavior::CollectReferences(EditorAssetDatabase& database, const std::string& path, std::vector<std::string>& outReferences)
	{
	}

}