#pragma once

#include "GuiWindow.h"

class Histogram : public GuiWindow {
public:
	std::shared_ptr<Image> currentImage;
	std::vector<unsigned char> grayData;
	Histogram(std::shared_ptr<Image> image)
		: currentImage(image) {}

	void frameAction() override;
	int itemCurrent = 0;
};