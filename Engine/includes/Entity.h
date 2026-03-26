#pragma once
#include <string>
#include <map>
#include <vector>
#include <IFixedUpdatable.h>
#include <IUpdatable.h>
#include <IDrawable.h>
#include <RectF.h>
#include <Vector2.h>
#include "Subject.h"
#include "Transform.h"
#include "PhysicsService.h"
#include "nlohmann/json.hpp"
#include "Component.h"

//#include "Engine.h"
//#include "Button.h"

using json = nlohmann::json;
namespace buki
{
	struct Entity final
	{
		virtual ~Entity() = default;
		Entity();
		Entity(std::string _name);
		virtual void Start();
		void Draw(float alpha);
		void FixedUpdate(float dt);
		void Update(float dt);
		void Destroy();

		Transform* GetTransform() { return transform; }
		void Initialize(Vector2 position, float rotation, Vector2 size);
		std::string GetName() { return m_Name; }
		inline void SetName(std::string name) { m_Name = name; }

		void ActivatePhysics();
		void DeactivatePhysics();
		inline bool HasPhysics() const { return m_Physics; }
		inline int GetZ() const { return zAxis; }
		inline void SetZ(const int _z) { zAxis = _z; }
		inline std::string GetLayer() const { return layer; }
		inline void SetLayer(const std::string _layer) { layer = _layer; }

		void Enable() { enabled = true; }
		void Disable() { enabled = false; }
		bool GetEnabled() const { return enabled; }

		json Serialize() const;
		void Deserialize(json _doc);
		void Set();
		
		template<typename T>
		T* AddComponent()
		{
			T* cmp = new T(this);
			m_ComponentByType.emplace(&typeid(T), cmp);

			IDrawable* drawable = dynamic_cast<IDrawable*>(cmp);
			if (drawable != nullptr)
			{
				m_Drawable.push_back(drawable);
			}

			IUpdatable* updatable = dynamic_cast<IUpdatable*>(cmp);
			if (updatable != nullptr)
			{
				m_Updatable.push_back(updatable);
			}

			IFixedUpdatable* Fixedupdatable = dynamic_cast<IFixedUpdatable*>(cmp);
			if (Fixedupdatable != nullptr)
			{
				m_FixedUpdatable.push_back(Fixedupdatable);
			}

			return cmp;
		}
		template<typename T>
		T* GetComponent()
		{
			const type_info* type = &typeid(T);
			auto it = m_ComponentByType.find(type);
			if (it != m_ComponentByType.end())
			{
				return dynamic_cast<T*>(it->second);
			}
			return nullptr;
		}

		template<typename T>
		T* GetComponentOfType()
		{
			const type_info* type = &typeid(T);
			for each (std::pair<const type_info*, Component*> cmp in m_ComponentByType)
			{
				if (dynamic_cast<T*>(cmp.second) && type != cmp.first)
				{
					return dynamic_cast<T*>(cmp.second);
				}
			}
			return nullptr;
		}

		template<typename T>
		std::vector<T*> GetAllComponentsOfType()
		{
			const type_info* type = &typeid(T);
			std::vector<T*> components;
			for each(std::pair<const type_info*, Component*> cmp in m_ComponentByType)
			{
				if (dynamic_cast<T*>(cmp.second) && type != cmp.first)
				{
					components.push_back(dynamic_cast<T*>(cmp.second));
				}
			}
			return components;
		}

	private:
		std::string m_Name;

		Transform* transform;
		
		std::map<const type_info*, Component*> m_ComponentByType;
		std::vector<IDrawable*> m_Drawable;
		std::vector<IUpdatable*> m_Updatable;
		std::vector<IFixedUpdatable*> m_FixedUpdatable;

		bool m_Physics = false;
		bool enabled = true;
		
		int zAxis = 0;
		std::string layer;
	};
}
