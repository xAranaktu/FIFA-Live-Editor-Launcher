#pragma once
#include <string> 

namespace core {
	class Config
	{
	public:
		std::string imgui_ini = "launcher_imgui.ini";

		Config();
		~Config();
	private:
		

	};
}

extern core::Config g_Config;