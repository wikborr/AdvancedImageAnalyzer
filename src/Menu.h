#pragma once
#include "GuiWindow.h"

class Menu: public GuiWindow{
public:
	bool closeWindow = false;
	std::shared_ptr<Image> currentImage;
	bool imageChanged = false;

	Menu(std::shared_ptr<Image> image)
		: currentImage(image) {}
	void frameAction() override;
	void fileMenu();
};