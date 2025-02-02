#pragma once

#include "ImageLoader.h"

class Window{
public:
	GLFWwindow* window;
	ImVec2 size;
	bool fullscreen = false;

	Window(ImVec2 size, bool fullscreen = false);
	void changeSize(ImVec2 size);
};