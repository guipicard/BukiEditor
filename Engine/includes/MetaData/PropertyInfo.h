#pragma once

#include <string>

namespace buki
{
	enum class PropertyType
	{
		Int,
		Float,
		Bool,
		String,
		StringList,
		ImageAsset,
		AudioAsset,
		EntityRef,
		PrefabRef
	};

	struct PropertyInfo
	{
		std::string name;
		PropertyType type = PropertyType::String;
		size_t offset = 0;

		bool hasMin = false;
		bool hasMax = false;
		float minValue = 0.0f;
		float maxValue = 0.0f;
		float dragSpeed = 1.0f;

		std::string filter;
	};
}

#define BUKI_PROP_INT(TYPE, FIELD) \
	{ #FIELD, buki::PropertyType::Int, offsetof(TYPE, FIELD) }

#define BUKI_PROP_FLOAT(TYPE, FIELD) \
	{ #FIELD, buki::PropertyType::Float, offsetof(TYPE, FIELD) }

#define BUKI_PROP_BOOL(TYPE, FIELD) \
	{ #FIELD, buki::PropertyType::Bool, offsetof(TYPE, FIELD) }

#define BUKI_PROP_STRING(TYPE, FIELD) \
	{ #FIELD, buki::PropertyType::String, offsetof(TYPE, FIELD) }

#define BUKI_PROP_FLOAT_RANGE(TYPE, FIELD, SPEED, MIN, MAX) \
	{ #FIELD, buki::PropertyType::Float, offsetof(TYPE, FIELD), SPEED, true, MIN, true, MAX }

#define BUKI_PROP_INT_RANGE(TYPE, FIELD, SPEED, MIN, MAX) \
	{ #FIELD, buki::PropertyType::Int, offsetof(TYPE, FIELD), SPEED, true, static_cast<float>(MIN), true, static_cast<float>(MAX) }

#define BUKI_PROP_STRING_LIST(TYPE, FIELD) \
	{ #FIELD, buki::PropertyType::StringList, offsetof(TYPE, FIELD) }

#define BUKI_PROP_ENTITY(TYPE, FIELD) \
	{ #FIELD, buki::PropertyType::EntityRef, offsetof(TYPE, FIELD) }

#define BUKI_PROP_PREFAB(TYPE, FIELD) \
	{ #FIELD, buki::PropertyType::PrefabRef, offsetof(TYPE, FIELD) }