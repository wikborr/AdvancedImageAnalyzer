#include "Filters.h"

void Filters::frameAction(){
	ImGui::Begin("Filters", nullptr);

	ImGui::InputInt("Size", &size);
	ImGui::NewLine();

	int count = 1;
	for(auto& i : this->convMatrix){
		for(auto& j : i){
			ImGui::PushItemWidth(50);
			ImGui::InputFloat((std::string("##")+std::to_string(count)).c_str(), &j);
			ImGui::PopItemWidth();
			count += 1;
			ImGui::SameLine();
		}
		ImGui::NewLine();
	}

	ImGui::NewLine();
	const char* filters[] = {"Custom", "Uniform Blur", "Gaussian Blur", "Sharpen", "Sobel", "Previt", "Roberts", "Laplace", "LoG"};
	ImGui::ListBox("##Filters", &currentFilter, filters, IM_ARRAYSIZE(filters), 9);

	ImGui::NewLine();
	ImGui::PushItemWidth(75);
	ImGui::InputFloat("Gauss variance", &gaussVariance, 0.0f, 0.0f, "%.3f");
	ImGui::PopItemWidth();

	ImGui::End();

	//calculations

	if(this->prevSize != this->size && this->size > 0)
		this->fixKernelSize();

	//different filters
	if(currentFilter != 0){
		if(currentFilter == 1){
			float blurValue = 1.f/pow(convMatrix.size(), 2);
			for(auto& i : convMatrix)
				for(auto& j : i)
					j = blurValue;
		}
		else if(currentFilter == 2){
			this->gauss2Kernel(convMatrix, this->gaussVariance);
		}
		else if(currentFilter == 3){
			this->size = 2;
			this->fixKernelSize();
			float gaussSum = 0;
			for(int i = 0; i < convMatrix.size(); i++)
				for(int j = 0; j < convMatrix.size(); j++){
					glm::vec2 position = glm::vec2((float)j - convMatrix.size() / 2, (float)i - convMatrix.size() / 2);
					convMatrix.at(i).at(j) = this->diffOfGauss(position, this->gaussVariance, 1.6f);
					gaussSum += convMatrix.at(i).at(j);
				}
			std::vector<std::vector<float>> laplacian = {
				{-1, -1, -1},
				{-1, 9, -1},
				{-1, -1, -1}
			};
			for(int i = 0; i < convMatrix.size(); i++)
				for(int j = 0; j < convMatrix.size(); j++)
					convMatrix.at(i).at(j) = (convMatrix.at(i).at(j) / gaussSum) + laplacian.at(i).at(j);
		}
		else if(currentFilter == 4){
			this->size = 2;
			this->fixKernelSize();
			std::vector<std::vector<float>> filter = {
				{0, 2, 2},
				{-2, 0, 2},
				{-2, -2, 0}
			};
			for(int i = 0; i < convMatrix.size(); i++)
				for(int j = 0; j < convMatrix.size(); j++)
					convMatrix.at(i).at(j) = filter.at(i).at(j);
		}
		else if(currentFilter == 5){
			this->size = 2;
			this->fixKernelSize();
			std::vector<std::vector<float>> filter = {
				{0, 1, 2},
				{-1, 0, 1},
				{-2, -1, 0}
			};
			for(int i = 0; i < convMatrix.size(); i++)
				for(int j = 0; j < convMatrix.size(); j++)
					convMatrix.at(i).at(j) = filter.at(i).at(j);
		}
		else if(currentFilter == 6){
			this->size = 2;
			this->fixKernelSize();
			std::vector<std::vector<float>> filter = {
				{0, 1, 0},
				{-1, 0, 0},
				{0, 0, 0}
			};
			for(int i = 0; i < convMatrix.size(); i++)
				for(int j = 0; j < convMatrix.size(); j++)
					convMatrix.at(i).at(j) = filter.at(i).at(j);
		}
		else if(currentFilter == 7){
			this->size = 2;
			this->fixKernelSize();
			std::vector<std::vector<float>> filter = {
				{-1, -1, -1},
				{-1, 8, -1},
				{-1, -1, -1}
			};
			for(int i = 0; i < convMatrix.size(); i++)
				for(int j = 0; j < convMatrix.size(); j++)
					convMatrix.at(i).at(j) = filter.at(i).at(j);
		}
		else if(currentFilter == 8){
			this->size = 2;
			this->fixKernelSize();
			float gaussSum = 0;
			for(int i = 0; i < convMatrix.size(); i++)
				for(int j = 0; j < convMatrix.size(); j++){
					glm::vec2 position = glm::vec2((float)j - convMatrix.size() / 2, (float)i - convMatrix.size() / 2);
					convMatrix.at(i).at(j) = this->diffOfGauss(position, this->gaussVariance, 1.6f);
					gaussSum += convMatrix.at(i).at(j);
				}
			std::vector<std::vector<float>> laplacian = {
				{-1, -1, -1},
				{-1, 8, -1},
				{-1, -1, -1}
			};
			for(int i = 0; i < convMatrix.size(); i++)
				for(int j = 0; j < convMatrix.size(); j++)
					convMatrix.at(i).at(j) = (convMatrix.at(i).at(j) /gaussSum) + laplacian.at(i).at(j);
		}
		currentFilter = 0;
	}

	this->applyFilter(this->currentImage, this->convMatrix);

	//gray data
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

void Filters::applyFilter(std::shared_ptr<Image> image, std::vector<std::vector<float>> kernel){
	std::unique_ptr<Image> imageCopy(new Image(image->data, image->width, image->height));
	for(int i = 0; i < image->data.size(); i += 4){
		if(i + 3 > image->data.size())
			break;
		glm::vec4 newPixelValues = glm::vec4(0.f);
		int pixelYCoord = (i / 4) / image->width;
		int pixelXCoord = (i / 4) - (pixelYCoord * image->width);
		for(int j = 0; j < kernel.size(); j++){
			int filterYCoord = pixelYCoord + (j - kernel.size() / 2);
			if(filterYCoord < 0 || filterYCoord >= image->height)
				continue;
			for(int k = 0; k < kernel.size(); k++){
				int filterXCoord = pixelXCoord + (k - kernel.size() / 2);
				if(filterXCoord < 0 || filterXCoord >= image->width)
					continue;
				newPixelValues.r += kernel.at(j).at(k) * imageCopy->data.at(filterYCoord * image->width * 4 + filterXCoord * 4);
				newPixelValues.g += kernel.at(j).at(k) * imageCopy->data.at(filterYCoord * image->width * 4 + filterXCoord * 4 + 1);
				newPixelValues.b += kernel.at(j).at(k) * imageCopy->data.at(filterYCoord * image->width * 4 + filterXCoord * 4 + 2);
				//newPixelValues.a += kernel.at(j).at(k) * imageCopy->data.at(filterYCoord * image->width * 4 + filterXCoord * 4 + 3);
			}
		}
		image->data.at(i) = std::min(static_cast<int>(newPixelValues.r), 255);
		image->data.at(i + 1) = std::min(static_cast<int>(newPixelValues.g), 255);
		image->data.at(i + 2) = std::min(static_cast<int>(newPixelValues.b), 255);
		//image->data.at(i+3) = std::min(static_cast<int>(newPixelValues.a), 255);
	}
}

float Filters::gaussValue(glm::vec2 pos, glm::vec2 var, glm::vec2 origin) {
	float pi = atan(1.f) * 4;
	float expValue = -0.5 * (pow(pos.x - origin.x, 2) / var.x + pow(pos.y - origin.y, 2) / var.y);
	return exp(expValue)/sqrt(2*pi);
}

void Filters::fixKernelSize(){
	this->currentFilter = 0;
	this->prevSize = this->size;
	convMatrix = std::vector<std::vector<float>>((size - 1) * 2 + 1, std::vector<float>((size - 1) * 2 + 1, 0.f));
	convMatrix.at(size - 1).at(size - 1) = 1.f;
}

float Filters::diffOfGauss(glm::vec2 pos, float variance, float k){
	float pi = atan(1.f) * 4;
	float value1 = exp(-0.5 * (pow(pos.x, 2) + pow(pos.y, 2)) / variance) / (2 * pi * variance);
	float value2 = exp(-0.5 * (pow(pos.x, 2) + pow(pos.y, 2)) / (variance * pow(k, 2))) / (2 * pi * variance * pow(k, 2));
	return value1 - value2;
}

void Filters::gauss2Kernel(std::vector<std::vector<float>>& kernel, float gaussVariance){
	float gaussSum = 0;
	for(int i = 0; i < kernel.size(); i++)
		for(int j = 0; j < kernel.size(); j++){
			glm::vec2 position = glm::vec2((float)j - kernel.size() / 2, (float)i - kernel.size() / 2);
			kernel.at(i).at(j) = gaussValue(position, glm::vec2(gaussVariance));
			gaussSum += kernel.at(i).at(j);
		}
	for(auto& i : kernel)
		for(auto& j : i)
			j /= gaussSum;
}