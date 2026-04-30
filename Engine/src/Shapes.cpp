#include "Shapes.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

json buki::Shapes::SerializeShapeDef(const ShapeDef& def) const
{
	json doc;

	doc["fillDraw"] = def.fillDraw;
	doc["shapeDraw"] = def.shapeDraw;

	doc["shapeColor"]["r"] = def.shapeColor.r;
	doc["shapeColor"]["g"] = def.shapeColor.g;
	doc["shapeColor"]["b"] = def.shapeColor.b;
	doc["shapeColor"]["a"] = def.shapeColor.a;

	doc["fillColor"]["r"] = def.fillColor.r;
	doc["fillColor"]["g"] = def.fillColor.g;
	doc["fillColor"]["b"] = def.fillColor.b;
	doc["fillColor"]["a"] = def.fillColor.a;

	doc["isSensor"] = def.isSensor;
	doc["filter"] = def.filter;
	doc["density"] = def.density;
	doc["friction"] = def.friction;
	doc["restitution"] = def.restitution;

	//doc["angleOffset"]["x"] = def.angleOffset;

	doc["positionOffset"]["x"] = def.positionOffset.x;
	doc["positionOffset"]["y"] = def.positionOffset.y;

	return doc;
}

void buki::Shapes::DeserializeShapeDef(const json& doc, ShapeDef& def) const
{
	def = DefaultShapeDef();

	def.fillDraw = doc.value("fillDraw", def.fillDraw);
	def.shapeDraw = doc.value("shapeDraw", def.shapeDraw);

	if (doc.contains("shapeColor"))
	{
		def.shapeColor.r = doc["shapeColor"].value("r", def.shapeColor.r);
		def.shapeColor.g = doc["shapeColor"].value("g", def.shapeColor.g);
		def.shapeColor.b = doc["shapeColor"].value("b", def.shapeColor.b);
		def.shapeColor.a = doc["shapeColor"].value("a", def.shapeColor.a);
	}

	if (doc.contains("fillColor"))
	{
		def.fillColor.r = doc["fillColor"].value("r", def.fillColor.r);
		def.fillColor.g = doc["fillColor"].value("g", def.fillColor.g);
		def.fillColor.b = doc["fillColor"].value("b", def.fillColor.b);
		def.fillColor.a = doc["fillColor"].value("a", def.fillColor.a);
	}

	def.isSensor = doc.value("isSensor", def.isSensor);
	def.filter = doc.value("filter", def.filter);
	def.density = doc.value("density", def.density);
	def.friction = doc.value("friction", def.friction);
	def.restitution = doc.value("restitution", def.restitution);

	//def.angleOffset = doc.value("angleOffset", def.angleOffset);


	if (doc.contains("positionOffset"))
	{
		def.positionOffset.x = doc["positionOffset"].value("x", def.positionOffset.x);
		def.positionOffset.y = doc["positionOffset"].value("y", def.positionOffset.y);
	}
}
