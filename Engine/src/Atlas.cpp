#pragma once
#include "Atlas.h"
#include "Entity.h"

using json = nlohmann::json;


buki::Atlas::Atlas(Entity* entity) : Sprite(entity)
{
}

void buki::Atlas::Draw(float alpha)
{
	//Log().LogMessage("Draw");
	if (tileWidth + tileHeight < 2) return;
	Vector2 entityPosition = m_Entity->GetTransform()->GetPosition();
	int tilesX = (int)destinations[0].size();
	int tilesY = (int)destinations.size();
	Vector2 fullSize = { sizeOffset.x * tilesX, sizeOffset.y * tilesY };
	entityPosition.x += positionOffset.x - (fullSize.x * 0.5f);
	entityPosition.y += positionOffset.y - (fullSize.y * 0.5f);
	Graphics().DrawTiledTexture(m_Id, destinations, sizeOffset, entityPosition, Color::WHITE);
}

json buki::Atlas::Serialize()
{
	json doc;
	doc = Sprite::Serialize();
	doc["tileWidth"] = tileWidth;
	doc["tileHeight"] = tileHeight;
	for (int i = 0; i < tileHeight; i++)
	{
		for (int j = 0; j < tileWidth; j++)
		{
			doc["destinations"][std::to_string(i)][std::to_string(j)]["x"] = destinations[i][j].x;
			doc["destinations"][std::to_string(i)][std::to_string(j)]["y"] = destinations[i][j].y;
			doc["destinations"][std::to_string(i)][std::to_string(j)]["w"] = destinations[i][j].w;
			doc["destinations"][std::to_string(i)][std::to_string(j)]["h"] = destinations[i][j].h;
		}
	}
	for each(std::pair<const std::string, RectI> src in sources)
	{
		doc["sources"][src.first] = {
			{"x", src.second.x},
			{"y", src.second.y},
			{"w", src.second.w},
			{"h", src.second.h}
		};
	}
	return doc;
}

void buki::Atlas::Deserialize(json _doc)
{
	Sprite::Deserialize(_doc);
	tileWidth = _doc["tileWidth"].get<int>();
	tileHeight = _doc["tileHeight"].get<int>();
	if (tileWidth + tileHeight < 2) return;
	SetTileSize(tileWidth, tileHeight);
	for (int i = 0; i < tileHeight; i++)
	{
		for (int j = 0; j < tileWidth; j++)
		{
			RectI dst = {
				_doc["destinations"][std::to_string(i)][std::to_string(j)]["x"].get<int>(),
				_doc["destinations"][std::to_string(i)][std::to_string(j)]["y"].get<int>(),
				_doc["destinations"][std::to_string(i)][std::to_string(j)]["w"].get<int>(),
				_doc["destinations"][std::to_string(i)][std::to_string(j)]["h"].get<int>()
			};
			destinations[i][j] = dst;
		}
	}
	for (auto& componentData : _doc["sources"].items())
	{
		const std::string& typeName = componentData.key();
		const json& componentJson = componentData.value();
		RectI rect;
		rect.x = componentJson["x"].get<int>();
		rect.y = componentJson["y"].get<int>();
		rect.w = componentJson["w"].get<int>();
		rect.h = componentJson["h"].get<int>();
		sources[typeName] = rect;
	}
}

void buki::Atlas::Set()
{
	Load(path);
}

void buki::Atlas::AddDestination(std::string source, const int x, const int y)
{
	destinations[y][x] = sources[source];
}

void buki::Atlas::AddSource(const std::string key, const RectI value)
{
	sources[key] = value;
}

void buki::Atlas::GetTextureSize(int* x, int* y)
{
	Graphics().GetTextureSize(m_Id, x, y);
}

void buki::Atlas::SetTileSize(const int x, const int y)
{
	tileWidth = x;
	tileHeight = y;
	destinations = std::vector<std::vector<RectI>>();
	for (int j = 0; j < y; j++)
	{
		destinations.push_back(std::vector<RectI>());
		for (int i = 0; i < x; i++)
		{
			destinations[j].push_back(RectI());
		}
	}
}
