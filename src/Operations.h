#pragma once

#include "GuiWindow.h"

class Operations: public GuiWindow{
public:
	std::shared_ptr<Image> currentImage;
	std::vector<unsigned char> grayData;
	Operations(std::shared_ptr<Image> image)
		: currentImage(image) {}

	void frameAction() override;
	void otsu();
	std::vector<std::vector<float>> gaussStdDevToKernel(float stdDev);
	int angleTo4Directions(float angle);
	int angleTo8Directions(float angle);
	bool shouldSupress(std::vector<unsigned char>& data, int width, std::vector<int>& angleData, int i);
	bool shouldHysteresis(std::vector<unsigned char>& data, int width, int i);

	bool doOtsu = false;
	bool binarization = false;
	bool edgeDetect = false;
	std::vector<unsigned char> edgeDetectData;
	bool houghTransform = false;

	int binThreshold = 127;
	int edgeCurrentStep = 4;
	float edgeGaussStdDev = 1.f;
	int edgeLowThreshold = 12;
	int edgeHighThreshold = 45;
	float houghThreshold = 0.05;
};
