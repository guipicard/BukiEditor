#pragma once
#include "EditorAssetEntries.h"

#include "Engine.h"
#include "Entity.h"
#include "Texture2D.h"
#include "EditorState.h"
#include "EditorViewportHelpers.h"

namespace
{
	bool MatchesPayloadType(const buki::BrowserEntry& entry, const char* payloadType)
	{
		if (payloadType == nullptr || payloadType[0] == '\0')
			return true;

		if (std::strcmp(payloadType, "FOLDER") == 0)
			return entry.type == buki::BrowserVisualType::Folder;

		if (std::strcmp(payloadType, "IMAGE") == 0)
			return entry.type == buki::BrowserVisualType::Image;

		if (std::strcmp(payloadType, "AUDIO") == 0)
			return entry.type == buki::BrowserVisualType::Audio;

		if (std::strcmp(payloadType, "PREFAB") == 0)
			return entry.type == buki::BrowserVisualType::Prefab;

		if (std::strcmp(payloadType, "SCENE") == 0)
			return entry.type == buki::BrowserVisualType::Scene;

		if (std::strcmp(payloadType, "ANIM") == 0)
			return entry.type == buki::BrowserVisualType::Anim;

		return false;
	}

	buki::BrowserEntry MakeBrowserEntry(const fs::directory_entry& dirEntry)
	{
		buki::BrowserEntry item;
		fs::path fullPath = fs::absolute(dirEntry.path()).lexically_normal();
		item.path = buki::ToAssetRelativePath(fullPath);
		item.displayName = fullPath.filename().string();
		if (dirEntry.is_directory())
		{
			item.type = buki::BrowserVisualType::Folder;
			item.payloadType = "FOLDER";
		}
		else if (buki::IsSceneFile(item.path))
		{
			item.type = buki::BrowserVisualType::Scene;
			item.payloadType = "SCENE";
		}
		else if (buki::IsPrefabFile(item.path))
		{
			item.type = buki::BrowserVisualType::Prefab;
			item.payloadType = "PREFAB";
		}
		else if (buki::IsImageFile(item.path))
		{
			item.type = buki::BrowserVisualType::Image;
			item.payloadType = "IMAGE";
		}
		else if (buki::IsAudioFile(item.path))
		{
			item.type = buki::BrowserVisualType::Audio;
			item.payloadType = "AUDIO";
		}
		else if (buki::IsAnimFile(item.path))
		{
			item.type = buki::BrowserVisualType::Anim;
			item.payloadType = "ANIM";
		}

		return item;
	}

}



void buki::SortBrowserEntries(std::vector<buki::BrowserEntry>& entries)
{
	std::sort(entries.begin(), entries.end(),
		[](const buki::BrowserEntry& a, const buki::BrowserEntry& b)
		{
			bool aDir = a.type == buki::BrowserVisualType::Folder;
			bool bDir = b.type == buki::BrowserVisualType::Folder;
			if (aDir != bDir)
				return aDir > bDir;

			return buki::ToLowerCopy(a.displayName) < buki::ToLowerCopy(b.displayName);
		});
}


bool buki::IsPrefabFile(const std::string& path)
{
	fs::path aspath = buki::ToAbsolutePath(path);
	return ToLowerCopy(aspath.extension().string()) == ".prefab";
}

bool buki::IsImageFile(const std::string& path)
{
	fs::path aspath = buki::ToAbsolutePath(path);
	const std::string ext = ToLowerCopy(aspath.extension().string());
	return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" ||
		ext == ".tga" || ext == ".gif" || ext == ".webp";
}

bool buki::IsAnimFile(const std::string& path)
{
	fs::path aspath = buki::ToAbsolutePath(path);
	return ToLowerCopy(aspath.extension().string()) == ".anim";
}

bool buki::IsAudioFile(const std::string& path)
{
	fs::path aspath = buki::ToAbsolutePath(path);
	const std::string ext = ToLowerCopy(aspath.extension().string());
	return ext == ".wav" || ext == ".mp3" || ext == ".ogg" || ext == ".flac";
}

bool buki::IsSceneFile(const std::string& path)
{
	fs::path aspath = buki::ToAbsolutePath(path);
	const std::string ext = aspath.extension().string();
	return ext == ".json" || ext == ".scene";
}

std::string buki::ToAssetRelativePath(const fs::path& fullPath)
{
	const std::string normalized = fullPath.lexically_normal().generic_string();
	const std::string lower = ToLowerCopy(normalized);

	const std::string assetsMarker = "assets/";
	const size_t assetsPos = lower.find(assetsMarker);
	if (assetsPos != std::string::npos)
		return "../Deployment/" + normalized.substr(assetsPos);

	const std::string deploymentMarker = "deployment/";
	const size_t deploymentPos = lower.find(deploymentMarker);
	if (deploymentPos != std::string::npos)
		return "../Deployment/" + normalized.substr(deploymentPos + deploymentMarker.size());

	return normalized;
}

fs::path buki::ToAbsolutePath(const std::string& assetRelativePath)
{
	return fs::absolute(fs::path("../Deployment") / assetRelativePath).lexically_normal();
}

std::string buki::DisplayNameForPath(const std::string& path)
{
	if (path.empty())
		return "None";

	return fs::path(path).filename().string();
}

std::string buki::FolderSuffixForPath(const std::string& path)
{
	const fs::path parent = fs::path(path).parent_path();
	if (parent.empty())
		return "";

	return parent.generic_string();
}

std::string buki::GetFileName(const std::string& path)
{
	return fs::absolute("../Deployment/" + path).filename().string();
}

std::string buki::ToLowerCopy(std::string value)
{
	std::transform(value.begin(), value.end(), value.begin(),
		[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return value;
}

buki::BrowserVisualType buki::GetBrowserVisualType(const std::string& path, bool isDirectory)
{
	if (isDirectory)
		return BrowserVisualType::Folder;

	if (IsSceneFile(path))
		return BrowserVisualType::Scene;

	if (IsPrefabFile(path))
		return BrowserVisualType::Prefab;

	if (IsAnimFile(path))
		return BrowserVisualType::Anim;

	if (IsAudioFile(path))
		return BrowserVisualType::Audio;

	if (IsImageFile(path))
		return BrowserVisualType::Image;

	return BrowserVisualType::File;
}

buki::EditorBrowserIcons& buki::GetEditorBrowserIcons()
{
	static EditorBrowserIcons icons;
	static bool loaded = false;

	if (!loaded)
	{
		auto& textures = Engine::Get().Textures();

		icons.folder = textures.Load("EditorImg/Folder.png");
		icons.scene = textures.Load("EditorImg/Network.png");
		icons.audio = textures.Load("EditorImg/Audio.png");
		icons.file = textures.Load("EditorImg/File.png");
		icons.prefab = textures.Load("EditorImg/Portrait.png");
		icons.image = textures.Load("EditorImg/Landscape.png");
		icons.anim = textures.Load("assets/Kenney/Game Icons/PNG/Black/2x/video.png");

		loaded = true;
	}

	return icons;
}

const buki::Texture2D* buki::TryGetPreviewTexture(const std::string& path)
{
	if (!IsImageFile(path))
		return nullptr;

	auto& textures = Engine::Get().Textures();
	Texture2D* texture = textures.Get(path);

	if (texture == nullptr)
		texture = textures.Load(path);

	if (texture == nullptr)
		texture = textures.Load(path, true);

	if (texture == nullptr || !texture->IsValid())
		return nullptr;

	return texture;
}

const buki::Texture2D* buki::GetBrowserThumbnail(const std::string& path, bool isDirectory)
{
	const BrowserVisualType type = GetBrowserVisualType(path, isDirectory);

	if (type == BrowserVisualType::Image)
	{
		if (const Texture2D* preview = TryGetPreviewTexture(path))
			return preview;
	}

	const EditorBrowserIcons& icons = GetEditorBrowserIcons();

	switch (type)
	{
	case BrowserVisualType::Folder: return icons.folder;
	case BrowserVisualType::Scene:  return icons.scene;
	case BrowserVisualType::Prefab: return icons.prefab;
	case BrowserVisualType::Audio:  return icons.audio;
	case BrowserVisualType::Image:  return icons.image;
	case BrowserVisualType::Anim:   return icons.anim;
	case BrowserVisualType::File:   return icons.file;
	default:                        return icons.file;
	}
}

std::vector<buki::BrowserEntry> buki::CollectBrowserEntries(const fs::path& root, bool recursive)
{
	std::vector<BrowserEntry> results;

	if (!fs::exists(root) || !fs::is_directory(root))
		return results;

	if (recursive)
	{
		for (const auto& entry : fs::recursive_directory_iterator(root))
		{
			results.push_back(MakeBrowserEntry(entry));
		}
	}
	else
	{
		for (const auto& entry : fs::directory_iterator(root))
		{
			results.push_back(MakeBrowserEntry(entry));
		}
	}

	SortBrowserEntries(results);
	return results;
}

std::vector<buki::BrowserEntry> buki::CollectBrowserEntries(const fs::path& root, const char* payloadType)
{
	std::vector<BrowserEntry> results;

	if (!fs::exists(root) || !fs::is_directory(root))
		return results;

	for (const auto& entry : fs::recursive_directory_iterator(root))
	{
		if (!entry.is_regular_file())
			continue;

		BrowserEntry item = MakeBrowserEntry(entry);
		if (!MatchesPayloadType(item, payloadType))
			continue;

		item.payloadType = payloadType != nullptr ? payloadType : "";
		results.push_back(std::move(item));
	}

	SortBrowserEntries(results);
	return results;
}

std::vector<std::string> buki::CollectAssetPaths(const fs::path& root, const char* payloadType)
{
	std::vector<std::string> results;
	const std::vector<BrowserEntry> entries = CollectBrowserEntries(root, payloadType);

	results.reserve(entries.size());
	for (const BrowserEntry& entry : entries)
	{
		results.push_back(ToAssetRelativePath(entry.path));
	}

	return results;
}

bool buki::ClearSceneSelection(EditorState& state)
{
	state.selectedEntity = nullptr;
	state.activeEntity = nullptr;
	state.selectedEntities.clear();
	return true;
}

bool buki::ClearPrefabSelection(EditorState& state)
{
	state.selectedPrefabEntity = nullptr;
	state.selectedPrefabPath.clear();
	return true;
}

ImTextureID buki::ToImGuiTextureID(std::uint32_t textureId)
{
	return static_cast<ImTextureID>(textureId);
}

bool buki::DrawBrowserTile(const BrowserEntry& entry, float thumbnailSize, bool selected)
{
	bool activated = false;

	ImGui::PushID(entry.path.c_str());
	ImGui::BeginGroup();

	const Texture2D* thumbnail = GetBrowserThumbnail(entry.path, entry.type == buki::BrowserVisualType::Folder);

	ImVec2 buttonSize(thumbnailSize, thumbnailSize);
	ImVec2 imageSize(thumbnailSize, thumbnailSize);
	ImVec2 padding(thumbnailSize / 8.0f, thumbnailSize / 8.0f);

	imageSize.x -= padding.x * 2;
	imageSize.y -= padding.y * 2;

	if (thumbnail != nullptr && thumbnail->height != 0 && thumbnail->width != 0 &&
		thumbnail->height != thumbnail->width)
	{
		const float aspect = static_cast<float>(thumbnail->width) / static_cast<float>(thumbnail->height);
		if (aspect > 1.0f)
		{
			imageSize.y /= aspect;
			padding.y = (padding.y + (thumbnailSize - imageSize.y)) * 0.5f;
		}
		else
		{
			imageSize.x *= aspect;
			padding.x = (padding.x + (thumbnailSize - imageSize.x)) * 0.5f;
		}
	}

	const ImVec4 transparent(0.0f, 0.0f, 0.0f, 0.0f);
	const ImVec4 selectedTint(1.0f, 1.0f, 1.0f, 1.0f);
	const ImVec4 normalTint(1.0f, 1.0f, 1.0f, 0.96f);

	ImGui::PushStyleColor(ImGuiCol_Button, transparent);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.08f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.14f));
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, selected ? 2.0f : 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);

	if (selected)
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.35f, 0.65f, 1.0f, 1.0f));
	else
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 0.10f));
	if (thumbnail != nullptr && thumbnail->IsValid())
	{
		//ImGui::Indent(padding.x-4.0f);
		activated = ImGui::ImageButton(
			"##thumb",
			buki::ToImGuiTextureID(thumbnail->id),
			imageSize,
			ImVec2(0.0f, 0.0f),
			ImVec2(1.0f, 1.0f),
			transparent,
			selected ? selectedTint : normalTint);
	}
	else
	{
		activated = ImGui::Button("##thumb_fallback", buttonSize);
	}

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		if (!entry.payloadType.empty())
		{
			const std::string pathStr = buki::ToAbsolutePath(entry.path).string();
			ImGui::SetDragDropPayload(entry.payloadType.c_str(), pathStr.c_str(), pathStr.size() + 1);
			ImGui::TextUnformatted(entry.displayName.c_str());
		}
		ImGui::EndDragDropSource();
	}

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar(2);

	ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + thumbnailSize);
	ImGui::TextWrapped("%s", entry.displayName.c_str());
	ImGui::PopTextWrapPos();

	ImGui::EndGroup();
	ImGui::PopID();

	return activated;
}

void buki::OnClick(EditorState& state, const BrowserEntry item)
{
	state.selectedItem = item.path;
	switch (item.type)
	{
	case BrowserVisualType::Folder:
		
		break;
	case BrowserVisualType::Scene:
		state.selectedScenePath = item.path;
		state.selectedPrefabPath.clear();
		state.selectedPrefabEntity = nullptr;
		break;
	case BrowserVisualType::Prefab:
		SelectPrefab(state, item.path);
		break;
	case BrowserVisualType::Anim:
		if (state.animationAssetEditor->Load(item.path))
		{
		}
		break;
	default:
		break;
	}
}

void buki::OnDoubleClick(EditorState& state, const BrowserEntry item)
{
	IWorld* world = nullptr;
	Vector2 sceneCamPos;
	Vector2 prefabCamPos;
	switch (item.type)
	{
	case BrowserVisualType::Folder:
		state.currentContentPath = item.path;
		break;
	case BrowserVisualType::Scene:
		sceneCamPos = state.scenePreviewSession.cameraSettings.position;
		state.selectedScenePath = item.path;
		state.requestSceneWindowFocus = true;
		world = Engine::Get().GetWorldPtr();
		if (world != nullptr && world->LoadScene(item.path))
		{
			ClearSceneSelection(state);
			state.activePrefabPreviewIndex = -1;
			state.selectedPrefabPath.clear();
			state.selectedPrefabEntity = nullptr;
			state.ClearSceneDirty();
			state.scenePreviewSession.cameraSettings.Deserialize(item.path);
			state.scenePreviewSession.path = item.path;
			state.selectedScenePath = item.path;
			Engine::Get().GetActiveCameraPtr()->position = { sceneCamPos.x, sceneCamPos.y };
			std::vector<Entity*> entities = world->GetEntitiesInWorld();
			for (Entity* entity : entities)
			{
				if (entity != nullptr) entity->Set();
			}
		}
		break;
	case BrowserVisualType::Prefab:
		SelectPrefab(state, item.path);
		prefabCamPos = state.prefabPreviewSessions[state.activePrefabPreviewIndex].cameraSettings.position;
		SelectPrefab(state, item.path);
		OpenPrefabPreview(state, item.path);
		state.prefabPreviewSessions[state.activePrefabPreviewIndex].cameraSettings.Deserialize(item.path);
		Engine::Get().GetActiveCameraPtr()->position = { prefabCamPos.x, prefabCamPos.y };
		break;
	case BrowserVisualType::Anim:
		if (state.animationAssetEditor->open)
		{
			state.animationAssetEditor->Load(item.path);
		}
		// TODO
		// OPEN IN OWN WINDOW
		break;
	}
}

void buki::OnRightClick(EditorState& state, const BrowserEntry item)
{
	switch (item.type)
	{

	case BrowserVisualType::Folder:
	{
		break;
	}
	case BrowserVisualType::Scene:
	{
		break;
	}
	case BrowserVisualType::Prefab:
	{

		if (ImGui::MenuItem("Instantiate"))
		{
			auto* world = Engine::Get().GetWorldPtr();
			if (world != nullptr)
			{
				world->InstantiatePrefab(item.path);
			}
		}
		if (ImGui::MenuItem("Preview"))
		{
			SelectPrefab(state, item.path);
			OpenPrefabPreview(state, item.path);
		}
		break;
	}
	case BrowserVisualType::Anim:
	{
		if (ImGui::MenuItem("Delete"))
		{
			// TODO 
			// INCLUDE IN UNIVERSAL RIGHT CLICK MENU 
		}
		break;
	}
	default:
		break;
	}
}


void buki::SelectPrefab(EditorState& state, const std::string& path)
{
	state.selectedEntity = nullptr;
	state.activeEntity = nullptr;
	state.selectedEntities.clear();
	state.animationAssetEditor->open = false;

	state.selectedPrefabPath = path;

	IWorld* world = Engine::Get().GetWorldPtr();
	state.selectedPrefabEntity = (world != nullptr)
		? world->GetOrLoadPrefabEntity(path)
		: nullptr;
}

bool buki::OpenPrefabPreview(EditorState& state, const std::string& path)
{
	IWorld* world = Engine::Get().GetWorldPtr();
	if (world == nullptr)
	{
		return false;
	}

	const fs::path absNormalized = fs::absolute("../Deployment/" + path).lexically_normal();
	const std::string& normalized = path;

	if (state.activePrefabPreviewIndex >= 0 &&
		state.activePrefabPreviewIndex < static_cast<int>(state.prefabPreviewSessions.size()))
	{
		state.prefabPreviewSessions[state.activePrefabPreviewIndex].focused = false;
	}

	for (size_t i = 0; i < state.prefabPreviewSessions.size(); ++i)
	{
		PrefabPreviewSession& session = state.prefabPreviewSessions[i];
		if (session.path == normalized)
		{
			session.open = true;
			session.focused = true;
			session.requestFocus = true;
			session.prefabEntity = world->GetOrLoadPrefabEntity(path);

			state.activePrefabPreviewIndex = static_cast<int>(i);
			state.selectedPrefabPath = normalized;
			state.selectedPrefabEntity = session.prefabEntity;
			return session.prefabEntity != nullptr;
		}
	}

	PrefabPreviewSession session;
	session.path = normalized;
	session.prefabEntity = world->GetOrLoadPrefabEntity(path);
	session.open = (session.prefabEntity != nullptr);
	session.focused = session.open;
	session.requestFocus = session.open;
	session.requestDockNextToScene = session.open;
	session.windowId = absNormalized.string();

	if (!session.open)
	{
		return false;
	}

	state.prefabPreviewSessions.push_back(session);
	state.activePrefabPreviewIndex = static_cast<int>(state.prefabPreviewSessions.size() - 1);
	state.selectedPrefabPath = normalized;
	state.selectedPrefabEntity = session.prefabEntity;
	return true;
}