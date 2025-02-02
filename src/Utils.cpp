#include "Utils.h"

namespace Utils{
	glm::ivec3 rbg2hsl(glm::ivec3 rgb){
		//conversion method used: https://gist.github.com/ciembor/1494530

		glm::vec3 result = glm::vec3(0.f);
		glm::vec3 fRGB = rgb;
		fRGB.x = std::max(std::min(int(fRGB.x), 255), 0);
		fRGB.y = std::max(std::min(int(fRGB.y), 255), 0);
		fRGB.z = std::max(std::min(int(fRGB.z), 255), 0);

		fRGB /= 255;

		float max = std::max(std::max(fRGB.r, fRGB.g), fRGB.b);
		float min = std::min(std::min(fRGB.r, fRGB.g), fRGB.b);

		result.x = result.y = result.z = (max + min) / 2;

		if(max == min) {
			result.x = result.y = 0; // achromatic
		}
		else {
			float d = max - min;
			result.y = (result.z > 0.5) ? d / (2 - max - min) : d / (max + min);

			if(max == rgb.r) {
				result.x = (rgb.g - rgb.b) / d + (rgb.g < rgb.b ? 6 : 0);
			}
			else if(max == rgb.g) {
				result.x = (rgb.b - rgb.r) / d + 2;
			}
			else if(max == rgb.b) {
				result.x = (rgb.r - rgb.g) / d + 4;
			}

			result.x /= 6;
		}

		result.x *= 360;
		result.y *= 100;
		result.z *= 100;

		return result;
	}


	glm::ivec3 hsl2rgb(glm::ivec3 hsl){
		//conversion method used: https://gist.github.com/ciembor/1494530

		glm::vec3 result = glm::vec3(0.f);
		glm::vec3 fHSL = hsl;

		fHSL.x /= 360;
		fHSL.y /= 100;
		fHSL.z /= 100;

		if(0 == fHSL.y) {
			result.r = result.g = result.b = fHSL.z; // achromatic
		}
		else {
			float q = fHSL.z < 0.5 ? fHSL.z * (1 + fHSL.y) : fHSL.z + fHSL.y - fHSL.z * fHSL.y;
			float p = 2 * fHSL.z - q;
			result.r = hue2rgb(p, q, fHSL.x + 1. / 3) * 255;
			result.g = hue2rgb(p, q, fHSL.x) * 255;
			result.b = hue2rgb(p, q, fHSL.x - 1. / 3) * 255;
		}

		return result;
	}

	float hue2rgb(float p, float q, float t) {
		//conversion method used: https://gist.github.com/ciembor/1494530

		if(t < 0)
			t += 1;
		if(t > 1)
			t -= 1;
		if(t < 1. / 6)
			return p + (q - p) * 6 * t;
		if(t < 1. / 2)
			return q;
		if(t < 2. / 3)
			return p + (q - p) * (2. / 3 - t) * 6;

		return p;
	}
}