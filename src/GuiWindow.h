#pragma once

#include "ImageLoader.h"
#include "Window.h"
#include "Utils.h"

class GuiWindow{
public:
	std::shared_ptr<Window> window;
	virtual void frameAction() = 0;
};