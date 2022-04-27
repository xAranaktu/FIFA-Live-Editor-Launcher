#pragma once
#include <config.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Main User Interface
class GUI
{
public:
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool show_main_window = true;
	bool show_demo_window = true;
	bool show_another_window = false;

	GUI();
	~GUI();

	void Init();
	void SetupImGUI();
	void Draw();

private:

};

extern GUI g_GUI;