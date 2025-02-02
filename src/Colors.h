#pragma once

#include "GuiWindow.h"

class Colors: public GuiWindow{
public:
	std::shared_ptr<Image> currentImage;
	std::vector<unsigned char> grayData;
	Colors(std::shared_ptr<Image> image)
		: currentImage(image) {}

	void frameAction() override;

	bool desaturated = false;
	bool negative = false;

	bool linContrast = false;
	float linA = 0.5;
	float linB = 85;
	bool powContrast = false;
	int pow = 2;
	bool logContrast = false;

	float hue = 0.f;
	float saturation = 0.f;
	float lightness = 0.f;
};