//#pragma once
//#include "Sprite.h"
//#include <map>
//
//using json = nlohmann::json;
//namespace buki
//{
//	//[SerializableComponent]
//	struct Atlas : public Sprite
//	{
//		virtual ~Atlas() = default;
//		Atlas(Entity* entity);
//		virtual void Draw(float alpha) override;
//
//		virtual json Serialize() override;
//		virtual void Deserialize(json _doc) override;
//		virtual void Set() override;
//
//		void AddDestination(std::string source, const int x, const int y);
//		void AddSource(const std::string key, const RectI value);
//		void GetTextureSize(int* x, int* y);
//		void SetTileSize(const int x, const int y);
//	private:
//		std::vector<std::vector<RectI>> destinations = std::vector<std::vector<RectI>>();
//		std::map<std::string, RectI> sources = std::map<std::string, RectI>();
//		int tileWidth = 0;
//		int tileHeight = 0;
//	};
//}
