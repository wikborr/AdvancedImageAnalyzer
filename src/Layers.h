#pragma once

#include "GuiWindow.h"

class Layers: public GuiWindow{
public:
	std::shared_ptr<Image> currentImage;
	Layers(std::shared_ptr<Image> image)
		: currentImage(image) {}

	void frameAction() override;
};