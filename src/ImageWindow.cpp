#include "ImageWindow.h"

void ImageWindow::frameAction(){
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove;
	ImGui::Begin("Image", nullptr, window_flags);

	ImGui::BeginChild("ImageView", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
	if(currentImage->data.size() > 0){
		ImVec2 imgSize = ImVec2(currentImage->width * scale, currentImage->height * scale);
		glBindTexture(GL_TEXTURE_2D, imageTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, currentImage->width, currentImage->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, currentImage->data.data());
		ImGui::SetCursorPos((ImVec2((ImGui::GetWindowSize().x - imgSize.x) * 0.5f, (ImGui::GetWindowSize().y - imgSize.y) * 0.5f)));
		ImGui::Image((void*)(intptr_t)imageTexture, ImVec2(imgSize.x, imgSize.y));
    }
	ImGui::EndChild();
    ImGui::End();

	ImGui::Begin("Scale", nullptr, ImGuiWindowFlags_NoMove);
	ImGui::SliderFloat("Scale", &scale, 0.001, 10);
	ImGui::End();
}