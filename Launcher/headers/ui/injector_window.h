#pragma once
#include <string>
#include <vector>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

namespace UIWindows {
	class Injector
	{
	public:
		enum DirectX {
			DX12 = 0,
			DX11 = 1
		};

		bool show = true;
		std::string window_name = "Injector";

		Injector();
		~Injector();
		void Draw(bool* p_open);
		void Dock(ImGuiID dock_id);
		const char* GetWindowName();

	private:
		int picked_direcx = DirectX::DX12;
		std::vector<std::string> DX_strings = {
			"DX12", "DX11"
		};
	};
}
