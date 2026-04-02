#pragma once
#include "Component.h"
#include "IDrawable.h"
#include "IUpdatable.h"
#include "nlohmann/json.hpp"
namespace buki
{
	using json = nlohmann::json;
	struct Entity;

	struct EditorEntity : public Component, public IDrawable, public IUpdatable
	{
		EditorEntity(Entity* e);
		virtual void Update(float dt) override;
		virtual void Draw(float alpha) override;
		virtual void Start() override;
		virtual void Destroy() override;

		virtual json Serialize() override;
		virtual void Deserialize(json _doc) override;
		virtual void Set() override;
		//virtual json Serialize() override;
		//virtual void Deserialize(json _doc) override;
		//virtual void Set() override;
		inline void SetSelected(bool state) { selected = state; }
		inline void SetScenePath(const std::string& path) { scenePath = path; }
		inline void AddComponentData(const std::string& componentName, const json& componentData) { componentsData[componentName] = componentData; }
	private:
		std::string scenePath = "";
		std::map<std::string, json> componentsData;
		bool selected = false;
	};
}