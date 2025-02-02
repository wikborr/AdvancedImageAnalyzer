#include "Menu.h"

void Menu::frameAction(){
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if(dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);

    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable){
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if(ImGui::BeginMenuBar()){
        if(ImGui::BeginMenu("File")){
            this->fileMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void Menu::fileMenu(){
    if(ImGui::MenuItem("Load image", nullptr)){
        nfdchar_t* outPath = nullptr;
        nfdresult_t result = NFD_OpenDialog("ppm,pbm,pgm,png,jpg", nullptr, &outPath);
        if(result == NFD_OKAY){
            ImageLoader::loadImage(this->currentImage, outPath);
            this->imageChanged = true;
            free(outPath);
        }
    }
	//auto saveFlags = ImGuiFl
	bool saveable = false;
	if(!currentImage->data.empty()){
		saveable = true;
	}
    if(ImGui::MenuItem("Save image", nullptr, nullptr, saveable)){
        nfdchar_t* savePath = nullptr;
        nfdresult_t result = NFD_SaveDialog("ppm,pbm,pgm,png,jpg", nullptr, &savePath);
        if(result == NFD_OKAY){
            ImageLoader::saveImage(this->currentImage, savePath);
            free(savePath);
        }
    }
    if(ImGui::MenuItem("Close", nullptr)){
        this->closeWindow = true;
    }
    ImGui::EndMenu();
}