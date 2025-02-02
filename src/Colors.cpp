#include "Colors.h"

void Colors::frameAction(){
	ImGui::Begin("Colors", nullptr);

	ImGui::Checkbox("Desaturate", &desaturated);
	ImGui::Checkbox("Negative", &negative);
	ImGui::Separator();

	ImGui::Checkbox("Lin contrast", &linContrast);
	if(linContrast){
		ImGui::InputFloat("a", &linA, 0.01f, 1.0f, "%.3f");
		ImGui::InputFloat("b", &linB, 0.01f, 1.0f, "%.3f");
	}
	ImGui::Checkbox("Pow contrast", &powContrast);
	if(powContrast){
		ImGui::InputInt("power", &pow, 1, 1);
	}
	ImGui::Checkbox("Log contrast", &logContrast);
	ImGui::Separator();

	ImGui::SliderFloat("Hue", &hue, -180, 180);
	ImGui::SliderFloat("Saturation", &saturation, -100, 100);
	ImGui::SliderFloat("Lightness", &lightness, -100, 100);

	ImGui::End();

	//calculations
	std::vector<unsigned char> newData = currentImage->data;
	
	if(linContrast){
		for(int i = 0; i < newData.size(); i++){
			if(i % 4 != 3){
				newData.at(i) = newData.at(i) * linA + linB;
			}
		}
	}
	if(powContrast){
		for(int i = 0; i < newData.size(); i++){
			if(i % 4 != 3){
				newData.at(i) = std::pow(newData.at(i), pow) / std::pow(255, pow-1);
			}
		}
	}
	if(logContrast){
		for(int i = 0; i < newData.size(); i++){
			if(i % 4 != 3){
				newData.at(i) = log10(newData.at(i)) * 106;
			}
		}
	}

	if(hue != 0.f || lightness != 0.f || saturation != 0.f){
		for(int i = 0; i < newData.size(); i++){
			if(i % 4 == 3){
				glm::ivec3 rgb = {newData.at(i - 3), newData.at(i - 2), newData.at(i - 1)};
				glm::ivec3 hsl = Utils::rbg2hsl(rgb);
				
				hsl.x += hue;
				if(hsl.x > 360) hsl.x -= 360;
				if(hsl.x < 0) hsl.x += 360;
				hsl.y += saturation;
				hsl.y = std::max(std::min(hsl.y, 100), 0);
				hsl.z += lightness;
				hsl.z = std::max(std::min(hsl.z, 100), 0);
				//std::cout << hsl.x << " " << hsl.y << " " << hsl.z << std::endl;

				rgb = Utils::hsl2rgb(hsl);
				newData.at(i - 3) = rgb.r;
				newData.at(i - 2) = rgb.g;
				newData.at(i - 1) = rgb.b;
			}
		}
	}

	std::vector<unsigned char> newDataCopy = newData;

	for(int i = 0; i < newData.size(); i++) {
		if(i % 4 == 3) {
			int max = std::max(static_cast<int>(newData.at(i - 3)), static_cast<int>(newData.at(i - 2)));
			max = std::max(static_cast<int>(newData.at(i - 1)), max);
			int min = std::min(static_cast<int>(newData.at(i - 3)), static_cast<int>(newData.at(i - 2)));
			min = std::min(static_cast<int>(newData.at(i - 1)), min);
			unsigned char gray = (max + min) / 2;
			newData.at(i - 3) = gray;
			newData.at(i - 2) = gray;
			newData.at(i - 1) = gray;
		}
	}
	grayData = newData;
	if(!desaturated){
		newData = newDataCopy;
	}
	if(negative){
		for(int i = 0; i < newData.size(); i++){
			if(i % 4 != 3){
				newData.at(i) = 255 - newData.at(i);
			}
		}
	}

	currentImage->data = newData;
}