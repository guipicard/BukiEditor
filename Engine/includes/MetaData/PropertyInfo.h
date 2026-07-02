#pragma once

#include <string>
#include <cstddef>
#include <type_traits>
#include <vector>

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
		FontAsset,
		EntityRef,
		PrefabRef,
		ImageAssetList,
		AudioAssetList,
		FontAssetList,
		Vector2,
		Color,
		RectF,
		EnumInt,
		EnumScriptBinding,
		TileList,
	};

	struct BukiField
	{
		std::string name;
		size_t offset;
		std::string key;
	};
	struct BukiProp
	{
		std::vector<BukiField> fields;
		PropertyType type;
		std::string group;
	};
	struct PropertyInfo
	{
		BukiProp prop;
		float minValue = 0.0f;
		float maxValue = 0.0f;
		float dragSpeed = 0.1f;
		bool hasMin = false;
		bool hasMax = false;
		std::vector<std::string> enumLabels = std::vector<std::string>();

		/* ---------- basic ---------- */

		// Basic constructor for non-numeric properties
		PropertyInfo(std::vector<BukiField> nestedFields, PropertyType propType, const char* group)
			: prop{ nestedFields, propType, group }
		{}
		
		/* ---------- specific ---------- */

		// Constructor for enum properties
		PropertyInfo(std::vector<BukiField> nestedFields, PropertyType propType, const char* group, const std::vector<const char*>& labels)
			: prop{ nestedFields, propType, group }
		{
			enumLabels.reserve(labels.size());
			for (const char* label : labels)
				enumLabels.emplace_back(label);
		}

		// Constructor for numeric properties with optional range and drag speed
		PropertyInfo(std::vector<BukiField> nestedFields, PropertyType propType, const char* group,
			float minV,
			float maxV,
			float drag,
			bool minSet,
			bool maxSet)
			: prop{ nestedFields, propType, group },
			minValue(minV),
			maxValue(maxV),
			dragSpeed(drag),
			hasMin(minSet),
			hasMax(maxSet)
		{}
	};
}

/* ---------- basic ---------- */
/* ---------- unnamed---------- */
// MACRO(TYPE, FIELD)
// PropertyInfo{ { { "FIELD", offsetof(TYPE, FIELD), NAME } }, PropertyType, group }
#define BUKI_PROP_INT( TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::Int, "" }

#define BUKI_PROP_FLOAT(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::Float, "" }

#define BUKI_PROP_BOOL(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::Bool, "" }

#define BUKI_PROP_STRING(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::String, "" }

#define BUKI_PROP_STRING_LIST(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::StringList, "" }	

#define BUKI_PROP_IMAGE(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::ImageAsset, "" }

#define BUKI_PROP_AUDIO(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::AudioAsset, "" }

#define BUKI_PROP_FONT(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::FontAsset, "" }

#define BUKI_PROP_ENTITY(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::EntityRef, "" }

#define BUKI_PROP_PREFAB(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::PrefabRef, "" }

#define BUKI_PROP_IMAGE_LIST(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::ImageAssetList, "" }

#define BUKI_PROP_AUDIO_LIST(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::AudioAssetList, "" }

#define BUKI_PROP_FONT_LIST(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::FontAssetList, "" }

#define BUKI_PROP_VECTOR2(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::Vector2, "" }

#define BUKI_PROP_COLOR(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::Color, "" }

#define BUKI_PROP_RECTF(TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), #FIELD } }, buki::PropertyType::RectF, "" }

/* ---------- named---------- */
// MACRO(NAME, TYPE, FIELD) ->
// PropertyInfo{ { { "FIELD", offsetof(TYPE, FIELD), NAME } }, PropertyType, group }
#define BUKI_PROP_INT_N( NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Int, "" }

#define BUKI_PROP_FLOAT_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Float, "" }

#define BUKI_PROP_BOOL_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Bool, "" }

#define BUKI_PROP_STRING_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::String, "" }

#define BUKI_PROP_STRING_LIST_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::StringList, "" }

#define BUKI_PROP_IMAGE_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::ImageAsset, "" }

#define BUKI_PROP_AUDIO_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::AudioAsset, "" }

#define BUKI_PROP_FONT_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::FontAsset, "" }

#define BUKI_PROP_ENTITY_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::EntityRef, "" }

#define BUKI_PROP_PREFAB_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::PrefabRef, "" }

#define BUKI_PROP_IMAGE_LIST_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::ImageAssetList, "" }

#define BUKI_PROP_AUDIO_LIST_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::AudioAssetList, "" }

#define BUKI_PROP_FONT_LIST_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::FontAssetList, "" }

#define BUKI_PROP_VECTOR2_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Vector2, "" }

#define BUKI_PROP_COLOR_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Color, "" }

#define BUKI_PROP_RECTF_N(NAME, TYPE, FIELD) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::RectF, "" }

/* ---------- ranged ---------- */

#define BUKI_PROP_FLOAT_RANGE_N(NAME, TYPE, FIELD, SPEED, MIN, MAX) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Float, "", \
		static_cast<float>(MIN), static_cast<float>(MAX), static_cast<float>(SPEED), true, true }

#define BUKI_PROP_INT_RANGE_N(NAME, TYPE, FIELD, SPEED, MIN, MAX) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Int, "", \
		static_cast<float>(MIN), static_cast<float>(MAX), static_cast<float>(SPEED), true, true }

#define BUKI_PROP_FLOAT_RANGE_GROUPED(NAME, TYPE, FIELD, MIN, MAX, SPEED, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Float, GROUP, \
		static_cast<float>(MIN), static_cast<float>(MAX), static_cast<float>(SPEED), true, true }

#define BUKI_PROP_INT_RANGE_GROUPED(NAME, TYPE, FIELD, MIN, MAX, SPEED, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Int, GROUP, \
		static_cast<float>(MIN), static_cast<float>(MAX), static_cast<float>(SPEED), true, true }

/* ---------- grouped ---------- */

#define BUKI_PROP_INT_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Int, GROUP }

#define BUKI_PROP_FLOAT_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Float, GROUP }

#define BUKI_PROP_BOOL_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Bool, GROUP }

#define BUKI_PROP_COLOR_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Color, GROUP }

#define BUKI_PROP_VECTOR2_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::Vector2, GROUP }

#define BUKI_PROP_STRING_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::String, GROUP }

#define BUKI_PROP_IMAGE_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::ImageAsset, GROUP }

#define BUKI_PROP_AUDIO_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::AudioAsset, GROUP }

#define BUKI_PROP_FONT_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::FontAsset, GROUP }	
		
#define BUKI_PROP_ENTITY_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::EntityRef, GROUP }

#define BUKI_PROP_PREFAB_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::PrefabRef, GROUP }

#define BUKI_PROP_RECTF_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::RectF, GROUP }

#define BUKI_PROP_STRINGLIST_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::StringList, GROUP }

#define BUKI_PROP_IMAGELIST_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::ImageAssetList, GROUP }

#define BUKI_PROP_AUDIOLIST_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::AudioAssetList, GROUP }

#define BUKI_PROP_FONTLIST_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::FontAssetList, GROUP }

/* ---------- ENUM ---------- */

#define BUKI_PROP_ENUM_INT_GROUPED(NAME, TYPE, FIELD, GROUP, ...) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::EnumInt, GROUP, { __VA_ARGS__ } }

/* ---------- script binding ---------- */
#define BUKI_PROP_ENUM_SCRIPT_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::EnumScriptBinding, GROUP }

/* ---------- script binding ---------- */
#define BUKI_PROP_TILE_LIST_GROUPED(NAME, TYPE, FIELD, GROUP) \
	buki::PropertyInfo{ { { #FIELD, offsetof(TYPE, FIELD), NAME } }, buki::PropertyType::TileList, GROUP }

/* ---------- nested ---------- */
/* ---------- offset ---------- */
#define BUKI_OFFSET_NESTED(TYPE, FIELD, SUBFIELD) \
	(offsetof(TYPE, FIELD) + offsetof(std::remove_reference_t<decltype(((TYPE*)0)->FIELD)>, SUBFIELD))

#define BUKI_OFFSET_NESTED2(TYPE, FIELD, SUBSTRUCT, SUBFIELD) \
	(offsetof(TYPE, FIELD) + \
	 BUKI_OFFSET_NESTED(TYPE, FIELD, SUBSTRUCT) + \
	 offsetof(std::remove_reference_t<decltype(((std::remove_reference_t<decltype(((TYPE*)0)->FIELD)>*)0)->SUBSTRUCT)>, SUBFIELD))

/* ---------- field ---------- */
// Fixed typo in "FIELD" macro definition name
#define BUKI_FIELD(TYPE, FIELD, NAME) \
	buki::BukiField{ #FIELD, offsetof(TYPE, FIELD), NAME }

// Cleaned up statement expressions and fixed NAME parameter mapping
#define BUKI_FIELD_NESTED(TYPE, FIELDNAME, FIELD, SUBNAME, SUBFIELD) \
	std::vector<buki::BukiField>{ \
		BUKI_FIELD(TYPE, FIELD, FIELDNAME), \
		buki::BukiField{ #SUBFIELD, BUKI_OFFSET_NESTED(TYPE, FIELD, SUBFIELD), SUBNAME } \
	} 

// Merges the parent field, the mid field, and the deep leaf field into a single flat vector
#define BUKI_FIELD_NESTED2(TYPE, FIELDNAME, FIELD, SUBSTRUCTNAME, SUBSTRUCTFIELD, SUBFIELDNAME, SUBFIELD) \
	std::vector<buki::BukiField>{ \
		BUKI_FIELD(TYPE, FIELD, FIELDNAME), \
		buki::BukiField{ #SUBSTRUCTFIELD, BUKI_OFFSET_NESTED(TYPE, FIELD, SUBSTRUCTFIELD), SUBSTRUCTNAME }, \
		buki::BukiField{ #SUBFIELD, BUKI_OFFSET_NESTED2(TYPE, FIELD, SUBSTRUCTFIELD, SUBFIELD), SUBFIELDNAME } \
	}

/* ---------- nested grouped ---------- */
#define BUKI_PROP_NESTED_BOOL_GROUPED(FIELDS, GROUP) \
	buki::PropertyInfo{ FIELDS, buki::PropertyType::Bool, GROUP }

#define BUKI_PROP_NESTED_INT_GROUPED(FIELDS, GROUP) \
	buki::PropertyInfo{ FIELDS, buki::PropertyType::Int, GROUP }

#define BUKI_PROP_NESTED_INT_RANGE_GROUPED(FIELDS, GROUP, MIN, MAX, SPEED) \
	buki::PropertyInfo{ FIELDS, buki::PropertyType::Int, GROUP, \
		static_cast<float>(MIN), static_cast<float>(MAX), static_cast<float>(SPEED), true, true }


#define BUKI_PROP_NESTED_FLOAT_GROUPED(FIELDS, GROUP) \
	buki::PropertyInfo{ FIELDS, buki::PropertyType::Float, GROUP }

#define BUKI_PROP_NESTED_FLOAT_RANGE_GROUPED(FIELDS, GROUP, MIN, MAX, SPEED) \
	buki::PropertyInfo{ FIELDS, buki::PropertyType::Float, GROUP, \
		static_cast<float>(MIN), static_cast<float>(MAX), static_cast<float>(SPEED), true, true }

#define BUKI_PROP_NESTED_VECTOR2_GROUPED(FIELDS, GROUP) \
	buki::PropertyInfo{ FIELDS, buki::PropertyType::Vector2, GROUP }

#define BUKI_PROP_NESTED_COLOR_GROUPED(FIELDS, GROUP) \
	buki::PropertyInfo{ FIELDS, buki::PropertyType::Color, GROUP }

#define BUKI_PROP_NESTED_STRING_GROUPED(FIELDS, GROUP) \
	buki::PropertyInfo{ FIELDS, buki::PropertyType::String, GROUP }