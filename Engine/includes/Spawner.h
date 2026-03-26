#pragma once
#include "Prototypes.h"
#include <cstdlib>
#include <string>
#include <map>

namespace buki {
	struct Spawner : public Component
	{
		Spawner(Entity* entity) {}
		virtual ~Spawner()
		{
			for (auto proto : m_Prototypes)
			{
				delete proto.second;
			}
		}
		virtual void AddPrototype(const std::string& name, Prototypes* proto)
		{
			m_Prototypes[name] = proto;
		}
		virtual Entity* Spawn(const std::string& name, Vector2 position = Vector2(), Vector2 size = Vector2(1.0f, 1.0f), float rotation = 0.0f)
		{
			if (m_Prototypes.count(name) > 0)
			{
				return m_Prototypes[name]->Clone(position, size, rotation);
			}
			return nullptr;
		}
		virtual json Serialize() override
		{
			json doc;
			for each(std::pair<const std::string, Prototypes*> proto in m_Prototypes)
			{
				//doc["prototypes"][proto.first] = proto.second->Serialize();
			}
			return doc;
		}
		virtual void Deserialize(json _doc) override
		{
			//for each (json prototypeData in _doc["prototypes"])
			//{
			//	//Prototypes* proto = new Prototypes();
			//	//proto->Deserialize(prototypeData);
			//	//m_Prototypes[prototypeData["name"].get<std::string>()] = proto;
			//}
		}
		virtual void Set() override {}
		virtual void Destroy() override
		{
			for (auto proto : m_Prototypes)
			{
				delete proto.second;
				proto.second = nullptr;
			}
			m_Prototypes.clear();
		}
	private:
		typedef std::map<std::string, Prototypes*> TProtoMap;
	protected:
		TProtoMap m_Prototypes;
	};
}
