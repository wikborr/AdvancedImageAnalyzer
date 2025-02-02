#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <stdlib.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include "nfd.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "glm/glm.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>
#include <cmath>

struct Image{
	Image(std::vector<unsigned char> data, int width, int height, int nrChannels = 4)
		: data(std::move(data)), width(width), height(height), nrChannels(nrChannels){}
	std::vector<unsigned char> data;
	int width = 0;	int height = 0;
	int nrChannels = 0;
};

namespace ImageLoader{
	bool loadImage(std::shared_ptr<Image> image, std::string path);
	bool saveImage(std::shared_ptr<Image> image, std::string path);
}