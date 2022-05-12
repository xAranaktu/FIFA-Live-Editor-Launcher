#include "injector_window.h"

namespace UIWindows {
	Injector::Injector() {}
	Injector::~Injector() {}

	void Injector::Draw(bool* p_open) {
		ImGui::Begin(GetWindowName(), p_open);


		ImGui::Text("Injection Delay (ms):");

		if (ImGui::CollapsingHeader("fifasetup")) {
			if (ImGui::Combo("DirectX", &picked_direcx, DX_strings)) {
				// TODO: Save DirectX in fifasetup.ini
			}
		}

		ImGui::End();
	}

	void Injector::Dock(ImGuiID dock_id) {
		ImGui::DockBuilderDockWindow(GetWindowName(), dock_id);
	}
	const char* Injector::GetWindowName() {
		return window_name.c_str(); 
	}
}
