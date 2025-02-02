#pragma once
#include "GuiWindow.h"

class ImageWindow: public GuiWindow{
public:
	std::shared_ptr<Image> currentImage;
	GLuint bgTexture = 0;
	GLuint imageTexture = 0;
	float scale = 1.f;
	
	ImageWindow(std::shared_ptr<Image> image)
		: currentImage(image) {}
	void frameAction() override;
};