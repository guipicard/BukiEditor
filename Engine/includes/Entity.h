#pragma once
#include <string>
#include <map>
#include <vector>
#include <IFixedUpdatable.h>
#include <IUpdatable.h>
#include <IDrawable.h>
#include "Subject.h"
#include "PhysicsService.h"
#include "Component.h"
#include "ComponentFactory.h"
#include "Memory.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace buki
{
	class Entity final
	{
	public:
		virtual ~Entity() = default;
		Entity();
		Entity(std::string _name);
		virtual void Start();
		void Draw(float alpha);
		void FixedUpdate(float dt);
		void Update(float dt);
		void Destroy();

		Transform& T() const;
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
		void SetEnable(bool _enabled) { enabled = _enabled; }
		bool IsEnabled() const { return enabled; }

		json Serialize() const;
		void Deserialize(json _doc);
		void Set();

		bool RemoveComponentByTypeName(const std::string& typeName);
		bool CanRemoveComponent(const std::string& typeName) const;

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

		bool AddComponentByTypeName(const std::string& name)
		{
			if (name.empty())
			{
				return false;
			}

			buki::Component* cmp = buki::ComponentFactory::CreateCmp(this, name, json{});
			if (cmp == nullptr)
			{
				return false;
			}

			m_ComponentByType.emplace(&typeid(*cmp), cmp);

			if (IDrawable* drawable = dynamic_cast<IDrawable*>(cmp))
			{
				m_Drawable.push_back(drawable);
			}

			if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(cmp))
			{
				m_Updatable.push_back(updatable);
			}

			if (IFixedUpdatable* fixedUpdatable = dynamic_cast<IFixedUpdatable*>(cmp))
			{
				m_FixedUpdatable.push_back(fixedUpdatable);
			}

			return true;
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
			for (std::pair<const type_info*, Component*> cmp : m_ComponentByType)
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
			for (std::pair<const type_info*, Component*> cmp : m_ComponentByType)
			{
				if (dynamic_cast<T*>(cmp.second) && type != cmp.first)
				{
					components.push_back(dynamic_cast<T*>(cmp.second));
				}
			}
			return components;
		}

		template<typename T>
		bool HasComponent()
		{
			const type_info* type = &typeid(T);
			return m_ComponentByType.find(type) != m_ComponentByType.end();
		}

		bool HasComponent(std::string name)
		{
			for (std::pair<const type_info*, Component*> cmp : m_ComponentByType)
			{
				std::string cmpTypeName = buki::ComponentFactory::GetTypeName(*cmp.first);

				if (cmpTypeName == name)
				{
					return true;
				}
			}

			return false;
		}

		Component* GetComponentByTypeName(const std::string& typeName)
		{
			for (auto& [type, component] : m_ComponentByType)
			{
				if (component == nullptr)
					continue;

				std::string current = ComponentFactory::GetTypeName(*type);
				if (current.empty())
					current = type->name();

				if (current == typeName)
					return component;
			}

			return nullptr;
		}

		std::map<const type_info*, Component*> GetComponents() const { return m_ComponentByType; }
	private:
		std::string m_Name;

		Scope<Transform> transform;

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
