#include "Shapes.h"

using json = nlohmann::json;

json buki::Shapes::Serialize()
{
	json doc;
	doc["fillDraw"] = fillDraw;
	doc["shapeDraw"] = shapeDraw;
	//doc["shapeColor"]["r"] = ShapeColor.r;
	//doc["shapeColor"]["g"] = ShapeColor.g;
	//doc["shapeColor"]["b"] = ShapeColor.b;
	//doc["shapeColor"]["a"] = ShapeColor.a;
	//doc["debugColor"]["r"] = DebugColor.r;
	//doc["debugColor"]["g"] = DebugColor.g;
	//doc["debugColor"]["b"] = DebugColor.b;
	//doc["debugColor"]["a"] = DebugColor.a;
	doc["Sensor"] = Sensor;
	doc["filter"] = filter;
	return doc;
}

void buki::Shapes::Deserialize(json _doc)
{
	fillDraw = _doc["fillDraw"].get<bool>();
	shapeDraw = _doc["shapeDraw"].get<bool>();
	//ShapeColor.r = _doc["shapeColor"]["r"].get<unsigned char>();
	//ShapeColor.g = _doc["shapeColor"]["g"].get<unsigned char>();
	//ShapeColor.b = _doc["shapeColor"]["b"].get<unsigned char>();
	//ShapeColor.a = _doc["shapeColor"]["a"].get<unsigned char>();
	//DebugColor.r = _doc["debugColor"]["r"].get<unsigned char>();
	//DebugColor.g = _doc["debugColor"]["g"].get<unsigned char>();
	//DebugColor.b = _doc["debugColor"]["b"].get<unsigned char>();
	//DebugColor.a = _doc["debugColor"]["a"].get<unsigned char>();
	Sensor = _doc["Sensor"].get<bool>();
	filter = _doc["filter"].get<int>();
}
