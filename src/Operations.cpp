#include "Operations.h"
#include "Filters.h"

void Operations::frameAction(){
	ImGui::Begin("Operations", nullptr);

	if(ImGui::Checkbox("Binarization", &binarization))
		this->otsu();
	if(binarization){
		ImGui::SliderInt("Threshold", &binThreshold, 0, 255);
		if(ImGui::Button("Optimize (Otsu)"))
			this->otsu();
	}
	ImGui::Separator();

	if(ImGui::Checkbox("Edge detection", &edgeDetect)){
		this->edgeCurrentStep = 4;
		//if(!edgeDetect)
		//	houghTransform = false;
	}
	if(edgeDetect){
		const char* steps[] = {"Step 0: Desaturation", "Step 1: Gauss", "Step 2-3: Gradient", "Step 4: Suppresion", "Step 5: Hysteresis"};
		if(ImGui::Combo("Step", &edgeCurrentStep, steps, IM_ARRAYSIZE(steps)))
			edgeDetectData.clear();
		ImGui::PushItemWidth(75);
		if(ImGui::InputFloat("Gauss standard deviation", &edgeGaussStdDev, 0.0f, 0.0f, "%.3f"))
			edgeDetectData.clear();
		ImGui::PushItemWidth(125);
		if(ImGui::SliderInt("Low threshold", &edgeLowThreshold, 0, 255))
			edgeDetectData.clear();
		if(ImGui::SliderInt("High threshold", &edgeHighThreshold, 0, 255))
			edgeDetectData.clear();
		ImGui::PopItemWidth();
	}
	ImGui::Separator();

	if(ImGui::Checkbox("Hough transform", &houghTransform))
		;//edgeDetect = (houghTransform) ? true : false;
	if(houghTransform){
		ImGui::PushItemWidth(200);
		ImGui::SliderFloat("##Threshold", &houghThreshold, 0.f, 0.1f);
		ImGui::PopItemWidth();
	}
	ImGui::Separator();

	ImGui::End();

	//--calculations--
	//binarization
	if(doOtsu){
		doOtsu = false;
		this->otsu();
	}
	if(binarization){
		currentImage->data = grayData;
		for(int i = 0; i < currentImage->data.size(); i+=4) {
			if(currentImage->data.at(i) > this->binThreshold){
				currentImage->data.at(i) = 255;
				currentImage->data.at(i + 1) = 255;
				currentImage->data.at(i + 2) = 255;
			}
			else{
				currentImage->data.at(i) = 0;
				currentImage->data.at(i + 1) = 0;
				currentImage->data.at(i + 2) = 0;
			}
		}
	}

	//edge detection
	if(edgeDetect && edgeDetectData.empty()){
		currentImage->data = grayData;
		if(edgeCurrentStep >= 1){
			//step 1: gauss
			std::vector<std::vector<float>> kernel = this->gaussStdDevToKernel(this->edgeGaussStdDev);
			Filters::applyFilter(this->currentImage, kernel);

			if(edgeCurrentStep >= 2){
				//step 2: sobel
				std::vector<unsigned char> dataCopy = this->currentImage->data;
				std::vector<std::vector<float>> xSobelKernel = {
					{-1, 0, 1},
					{-2, 0, 2},
					{-1, 0, 1}
				};
				Filters::applyFilter(this->currentImage, xSobelKernel);
				std::vector<unsigned char> xGrad = this->currentImage->data;
				this->currentImage->data = dataCopy;

				std::vector<std::vector<float>> ySobelKernel = {
					{1, 2, 1},
					{0, 0, 0},
					{-1, -2, -1}
				};
				Filters::applyFilter(this->currentImage, ySobelKernel);
				std::vector<unsigned char> yGrad = this->currentImage->data;
				this->currentImage->data = dataCopy;

				//step 3: magnitude and angle
				std::vector<int> angleData;
				for(int i = 0; i < this->currentImage->data.size(); i += 4){
					unsigned char magnitude = std::min(static_cast<int>(sqrt(pow(static_cast<int>(xGrad.at(i)), 2) + pow(static_cast<int>(yGrad.at(i)), 2))), 255);
					float angle = std::atan2(static_cast<int>(yGrad.at(i)), static_cast<int>(xGrad.at(i))) * 180 / (atan(1.f) * 4);
					angleData.push_back(this->angleTo4Directions(angle));
					for(int j = 0; j < 3; j++)
						currentImage->data.at(i + j) = magnitude;
				}

				if(edgeCurrentStep >= 3){
					//step 4: non maxima suppresion
					std::vector<unsigned char> dataCopy = currentImage->data;
					for(int i = 0; i < this->currentImage->data.size(); i += 4)
						if(this->shouldSupress(dataCopy, currentImage->width, angleData, i))
							for(int j = 0; j < 3; j++)
								currentImage->data.at(i + j) = 0;

					if(edgeCurrentStep >= 4){
						//step 5: hysteresis
						std::vector<unsigned char> dataCopy = currentImage->data;
						for(int i = 0; i < this->currentImage->data.size(); i += 4){
							if(this->shouldHysteresis(dataCopy, currentImage->width, i)){
								for(int j = 0; j < 3; j++)
									currentImage->data.at(i + j) = 255;
							}
							else{
								for(int j = 0; j < 3; j++)
									currentImage->data.at(i + j) = 0;
							}
						}
					}
				}
			}
			this->edgeDetectData = this->currentImage->data;
		}
	}
	else if(!edgeDetect && !edgeDetectData.empty())
		edgeDetectData.clear();
	else if(edgeDetect && !edgeDetectData.empty())
		this->currentImage->data = edgeDetectData;
	
	//hough transform
	if(houghTransform && !currentImage->data.empty()){
		if(!edgeDetect)
			currentImage->data = grayData;
		int maxRadius = sqrt(pow(currentImage->width, 2) + pow(currentImage->height, 2));
		std::vector<std::vector<int>> accumulationTable; //x - angles, y - radius
		accumulationTable = std::vector<std::vector<int>>(2 * maxRadius - 1, std::vector<int>(180, 0));
		std::vector<float> sins, coss;
		for(int i = 0; i < 180; i++){
			sins.push_back(sin(i));
			coss.push_back(cos(i));
		}

		for(int i = 0; i < this->currentImage->data.size(); i += 4){
			if(currentImage->data.at(i) > 127){
				int pixelYCoord = (i / 4) / currentImage->width;
				int pixelXCoord = (i / 4) - (pixelYCoord * currentImage->width);
				for(int j = 0; j < 180; j++){
					int radius = std::min(static_cast<int>(round(pixelXCoord * sins.at(j) + pixelYCoord * coss.at(j) + maxRadius)), 2 * maxRadius - 2);
					accumulationTable.at(radius).at(j) += 1;
				}
			}
		}

		int dataSize = currentImage->data.size() / 4;
		for(int i = 0; i < 2 * maxRadius - 1; i++){
			for(int j = 0; j < 180; j++){
				if(static_cast<float>(accumulationTable.at(i).at(j)) / dataSize >= this->houghThreshold){
					float radius = i - maxRadius;
					glm::vec2 lineOrigin = {radius*coss.at(j), radius*sins.at(j)};
					glm::vec2 lineVector = {-lineOrigin.y, -lineOrigin.x};
					lineVector = glm::normalize(lineVector);
					bool breaks[2] = {false, false};
					bool onScreenOnce = false;
					for(int k = 0; !breaks[0] || !breaks[1]; k++){
						glm::vec2 progress = lineVector * static_cast<float>(k);
						if(!breaks[0]){
							glm::ivec2 point = glm::ivec2(lineOrigin.x + progress.x, lineOrigin.y + progress.y);
							int imageDataInd = (currentImage->height - point.y) * currentImage->width * 4 + point.x * 4;
							if(imageDataInd >= currentImage->data.size()){
								if(onScreenOnce || k > 1000)
									breaks[0] = true;
							}
							else{
								onScreenOnce = true;
								currentImage->data.at(imageDataInd) = 255;
								currentImage->data.at(imageDataInd + 1) = 0;
								currentImage->data.at(imageDataInd + 2) = 0;
							}
						}
						if(!breaks[1]){
							glm::ivec2 point = glm::ivec2(lineOrigin.x - progress.x, lineOrigin.y - progress.y);
							int imageDataInd = (currentImage->height - point.y) * currentImage->width * 4 + point.x * 4;
							if(imageDataInd >= currentImage->data.size()){
								if(onScreenOnce || k > 1000)
									breaks[1] = true;
							}
							else{
								onScreenOnce = true;
								currentImage->data.at(imageDataInd) = 255;
								currentImage->data.at(imageDataInd + 1) = 0;
								currentImage->data.at(imageDataInd + 2) = 0;
							}
						}
					}
				}
			}
		}
	}

	//---gray data---
	std::vector<unsigned char> newGrayData = currentImage->data;
	for(int i = 0; i < newGrayData.size(); i++) {
		if(i % 4 == 3) {
			int max = std::max(static_cast<int>(newGrayData.at(i - 3)), static_cast<int>(newGrayData.at(i - 2)));
			max = std::max(static_cast<int>(newGrayData.at(i - 1)), max);
			int min = std::min(static_cast<int>(newGrayData.at(i - 3)), static_cast<int>(newGrayData.at(i - 2)));
			min = std::min(static_cast<int>(newGrayData.at(i - 1)), min);
			unsigned char gray = (max + min) / 2;
			newGrayData.at(i - 3) = gray;
			newGrayData.at(i - 2) = gray;
			newGrayData.at(i - 1) = gray;
		}
	}
	this->grayData = newGrayData;
}

void Operations::otsu(){
	float grayHistArray[256];
	memset(grayHistArray, 0.f, sizeof(grayHistArray));
	for(int j = 0; j < grayData.size(); j++) {
		if(j % 4 == 0) {
			int i = grayData.at(j);
			if(i > 255)
				i = 255;
			grayHistArray[i] += 1;
		}
	}
	std::vector<float> histogramData = std::vector<float>(std::begin(grayHistArray), std::end(grayHistArray));

	std::array<float, 256> variants{};
	int pixelCount = currentImage->width * currentImage->height;
	int prob0 = 0;
	int mean0 = 0;
	float maxValue = 0.f;
	int maxIndex = 0;
	int mean1 = 0;
	for(int i = 0; i < 256; i++) {
		mean1 += i * histogramData.at(i);
	}
	for(int i = 0; i < 256; i++){
		int prob1 = pixelCount - prob0;
		if(prob0 > 0 && prob1 > 0){
			float m1 = static_cast<float>(mean1 - mean0) / prob1;
			float progValue = prob0 * prob1 * pow((static_cast<double>(mean0) / prob0) - m1, 2);
			if(progValue > maxValue){
				maxValue = progValue;
				maxIndex = i;
			}
		}
		prob0 += histogramData.at(i);
		mean0 += i * histogramData.at(i);
	}
	this->binThreshold = maxIndex;
}

std::vector<std::vector<float>> Operations::gaussStdDevToKernel(float stdDev){
	float var = pow(stdDev, 2);
	int kernelSize = std::max(3.f, stdDev * 4 + 1);
	if(kernelSize % 2 == 0)
		kernelSize += 1;
	std::vector<std::vector<float>> kernel = std::vector<std::vector<float>>(kernelSize, std::vector<float>(kernelSize, 0.f));
	Filters::gauss2Kernel(kernel, var);
	return kernel;
}

int Operations::angleTo4Directions(float angle){
	if((angle >= 0.f && angle <= 22.5f) || (angle >= 157.5f && angle <= 180.f))
		return 0;
	else if(angle >= 22.5f && angle <= 67.5f)
		return 45;
	else if(angle >= 67.5f && angle <= 112.5f)
		return 90;
	else if(angle >= 112.5f && angle <= 157.5f)
		return 135;
	else
		return 0;
}

bool Operations::shouldSupress(std::vector<unsigned char>& data, int width, std::vector<int>& angleData, int i){
	int angle = angleData.at(i / 4);
	int sideInd[2] = {0, 0};
	if(angle == 0){
		sideInd[0] = i - 4;
		sideInd[1] = i + 4;
	}
	else if(angle == 45){
		sideInd[0] = i + 4 - width * 4;
		sideInd[1] = i - 4 + width * 4;
	}
	else if(angle == 90){
		sideInd[0] = i - width * 4;
		sideInd[1] = i + width * 4;
	}
	else if(angle == 135){
		sideInd[0] = i - 4 - width * 4;
		sideInd[1] = i + 4 + width * 4;
	}

	bool sideSmall[2] = {true, true};
	for(int j = 0; j < 2; j++)
		if(sideInd[j] > 0 && sideInd[j] < data.size() && data.at(sideInd[j]) >= data.at(i))
			sideSmall[j] = false;
	return !(sideSmall[0] || sideSmall[1]);
}

bool Operations::shouldHysteresis(std::vector<unsigned char>& data, int width, int i){
	if(data.at(i) < edgeLowThreshold)
		return false;
	else if(data.at(i) >= edgeHighThreshold)
		return true;
	else{
		std::vector<std::vector<float>> kernel = {
			{0, 4, 0},
			{4, 0, 4},
			{0, 4, 0}
		};
		float finalValue = 0;
		int pixelYCoord = (i / 4) / width;
		int pixelXCoord = (i / 4) - (pixelYCoord * width);
		for(int j = 0; j < kernel.size(); j++){
			int filterYCoord = pixelYCoord + (j - kernel.size() / 2);
			if(filterYCoord < 0 || filterYCoord*width*4 + 1 > data.size())
				continue;
			for(int k = 0; k < kernel.size(); k++){
				int filterXCoord = pixelXCoord + (k - kernel.size() / 2);
				if(filterXCoord < 0 || filterXCoord >= width)
					continue;
				finalValue += kernel.at(j).at(k) * data.at(filterYCoord * width * 4 + filterXCoord * 4);
			}
		}
		if(finalValue > edgeHighThreshold)
			return true;
		else
			return false;
	}
}

int Operations::angleTo8Directions(float angle){
	if((angle >= -22.5f && angle <= 22.5f))
		return 0;
	else if(angle >= 22.5f && angle <= 67.5f)
		return 45;
	else if(angle >= 67.5f && angle <= 112.5f)
		return 90;
	else if(angle >= 112.5f && angle <= 157.5f)
		return 135;
	else if(angle >= 157.5f && angle <= -157.5f)
		return 180;
	else if(angle >= -157.5f && angle <= -112.5f)
		return -135;
	else if(angle >= -112.5f && angle <= -67.5f)
		return -90;
	else if(angle >= -67.5f && angle <= -22.5f)
		return -45;
	else
		return 0;
}