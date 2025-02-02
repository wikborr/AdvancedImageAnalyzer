#include "Histogram.h"

void Histogram::frameAction() {
	ImGui::Begin("Histogram", nullptr);

	const char* items[] = { "Gray", "Red", "Green", "Blue"};
	ImGui::Combo(" ", &itemCurrent, items, IM_ARRAYSIZE(items));

	std::array<int, 100> valueArray{};
	
	if (currentImage->data.size() > 0) {
		if (itemCurrent == 0) {
			for (int j = 0; j < grayData.size(); j++) {
				if (j % 4 == 0) {
					int i = std::floor(static_cast<float>(grayData.at(j)) / 2.55f);
					if (i > 99) {
						i = 99;
					}
					valueArray.at(i) += 1;
				}
			}
		}
		else {
			for (int j = 0; j < currentImage->data.size(); j++) {
				if (j % 4 == itemCurrent - 1) {
					int i = std::floor(static_cast<float>(currentImage->data.at(j)) / 2.55);
					if (i > 99) {
						i = 99;
					}
					valueArray.at(i) += 1;
				}
			}
		}
	}

	float histogramArray[100];
	float maxValue = 0.f;
	for (int i = 0; i < 100; i++) {
		histogramArray[i] = static_cast<float>(valueArray.at(i)) / ( currentImage->width * currentImage->height );
		if(histogramArray[i] > maxValue)
			maxValue = histogramArray[i];
	}
	ImGui::PlotHistogram(" ", histogramArray, IM_ARRAYSIZE(histogramArray), 0, nullptr, 0.f, std::min(maxValue, 1.f), ImVec2(250.f, 200.f));

	ImGui::End();
}