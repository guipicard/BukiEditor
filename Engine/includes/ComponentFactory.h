#include <functional>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

	class Entity; // forward

	class ComponentFactory
	{
	public:
		using CreateFn = std::function<void(buki::Entity*, const json&)>;

		static void Register(const std::string& typeName, CreateFn fn, const std::type_info& type) {
			GetMap()[typeName] = std::move(fn);
			GetReverseMap()[&type] = typeName;
		}

		static bool Create(buki::Entity* e, const std::string& typeName, const json& j) {
			auto& map = GetMap();
			auto it = map.find(typeName);
			if (it == map.end())
				return false; // unknown component type
			it->second(e, j);
			return true;
		}

		static std::string GetTypeName(const std::type_info& type) {
			auto& rmap = GetReverseMap();
			auto it = rmap.find(&type);
			return it != rmap.end() ? it->second : "";
		}

		static const std::type_info* GetType(const std::string& type) {
			auto& rmap = GetReverseMap();
			for (auto& pair : rmap) {
				if (pair.second == type) {
					return pair.first;
				}
			}
			return nullptr;
		}

	private:
		static std::unordered_map<std::string, CreateFn>& GetMap() {
			static std::unordered_map<std::string, CreateFn> map;
			return map;
		}
		static std::unordered_map<const std::type_info*, std::string>& GetReverseMap() {
			static std::unordered_map<const std::type_info*, std::string> reverseMap;
			return reverseMap;
		}
	};
