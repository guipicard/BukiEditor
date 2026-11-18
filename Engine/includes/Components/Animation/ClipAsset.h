#pragma once

#include "AnimationTypes.h"

#include <string>
#include <vector>

namespace buki
{
	struct ClipAsset
	{
		
		int version = 1;
		std::string name;
		std::string path;
		bool loop = true;
		std::vector<SpriteFrame> frames;

		bool LoadFromFile(const std::string& _path);
		bool SaveToFile(const std::string& _path) const;
	};
}
