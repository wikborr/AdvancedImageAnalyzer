#include "Window.h"

Window::Window(ImVec2 size, bool fullscreen)
	: size(size), fullscreen(fullscreen) {
	this->window = glfwCreateWindow(size.x, size.y, "Advanced Image Analyzer", NULL, NULL);
}

void Window::changeSize(ImVec2 size){
	glfwSetWindowSize(this->window, size.x, size.y);
}