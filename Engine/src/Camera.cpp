#pragma once
#include "Camera.h"
#include "Engine.h"
#include "Entity.h"
#include "BukiContainers.h"

using json = nlohmann::json;

buki::Camera::Camera()
{
}

buki::Camera::~Camera()
{
}

buki::Vector2 buki::Camera::GetPosition() const
{
	return position;
}

void buki::Camera::SetPosition(const Vector2 pos) 
{
	position = pos;
}

json buki::Camera::Serialize()
{
	json doc;
	doc["x"] = position.x;
	doc["y"] = position.y;
	doc["width"] = width;
	doc["height"] = height;
	return doc;
}

void buki::Camera::Deserialize(json _doc)
{
	position.x = _doc["x"].get<float>();
	position.y = _doc["y"].get<float>();
	width = _doc["width"].get<int>();
	height = _doc["height"].get<int>();
}
