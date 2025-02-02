#pragma once

#include "ImageLoader.h"

namespace Utils{
	glm::ivec3 rbg2hsl(glm::ivec3 rgb);
	glm::ivec3 hsl2rgb(glm::ivec3 hsl);
	float hue2rgb(float p, float q, float t);
}