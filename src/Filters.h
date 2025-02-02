#pragma once

#include "GuiWindow.h"

class Filters: public GuiWindow{
public:
	std::shared_ptr<Image> currentImage;
	std::vector<unsigned char> grayData;
	Filters(std::shared_ptr<Image> image)
		: currentImage(image) {}

	void frameAction() override;
	static void applyFilter(std::shared_ptr<Image> image, std::vector<std::vector<float>> kernel);
	static float gaussValue(glm::vec2 position, glm::vec2 variance, glm::vec2 origin = glm::vec2(0.f));
	void fixKernelSize();
	float diffOfGauss(glm::vec2 position, float variance, float k);
	static void gauss2Kernel(std::vector<std::vector<float>>& kernel, float gaussVariance);

	int prevSize = 0;
	int size = 2;
	int currentFilter = 0;
	float gaussVariance = 1;
	std::vector<std::vector<float>> convMatrix;
};