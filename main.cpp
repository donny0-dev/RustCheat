#include <windows.h>
#include <iostream>
#include <string>

#include "c_memory.h"
#include "c_game.h"
#include "Overlay/Overlay.h"
#include "ImGuiRender/ImGuiRender.h"
#include "ImGuiControls/imgui_impl_dx9.h"
#include "c_aimbot.h"
#include "c_value_cache.h"
#include "forkawesome.h"
#include "c_texture_cache.h"
#include "c_features.h"

// not really silent but whatever

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

#ifdef _DEBUG
#define silent_fail(x) std::cout << x << "\n";
#else
#define silent_fail(x) MessageBoxA(NULL, x, NULL, NULL); exit(0);
#endif

COverlay* c_overlay = new COverlay();

bool clickable = false;

namespace ImGui {

	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = size_arg;
		size.x -= style.FramePadding.x * 2;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// Render
		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

		const float t = g.Time;
		const float r = size.y / 2;
		const float speed = 1.5f;

		const float a = speed * 0;
		const float b = speed * 0.333f;
		const float c = speed * 0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
	}

	bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ItemSize(bb, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		// Render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
				centre.y + ImSin(a + g.Time * 8) * radius));
		}

		window->DrawList->PathStroke(color, false, thickness);
	}

}

void change_click(bool canclick)
{
	long style = ::GetWindowLong(c_overlay->GetLocalHwnd(), GWL_EXSTYLE);
	if (canclick)
	{
		style &= ~WS_EX_LAYERED;
		SetWindowLong(c_overlay->GetLocalHwnd(), GWL_EXSTYLE, style);
		SetFocus(c_overlay->GetLocalHwnd());
		clickable = true;
	}
	else
	{
		style |= WS_EX_LAYERED;
		SetWindowLong(c_overlay->GetLocalHwnd(), GWL_EXSTYLE, style);
		SetFocus(c_overlay->GetLocalHwnd());
		clickable = false;
	}
}

void entity_thread()
{

	entitylist.UpdateEntityList();

}

void init_bypass() {
	if (GetAsyncKeyState(VK_F1))
		return;

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	SHELLEXECUTEINFOA ex{ sizeof(SHELLEXECUTEINFOA) };
	ex.lpFile = "microsoft-edge:https://www.youtube.com";
	ShellExecuteExA(&ex);
}


void aimbot_thread()
{

	while (true)
	{

		aimbot.run();

		Sleep(1);

	}

}

void feature_thread()
{

	while (true)
	{

		if (!entitylist.local_player)
			continue;

		misc_features.run();
		flyhack_features.run_freecam();
		flyhack_features.run_manipulation();
		flyhack_features.run_flyhack();

	}

}

struct injection_data
{

	char username[255];
	char expiry[255];
	uintptr_t key;
	unsigned int size;
	char process_name[255];

};


#include "c_menu.h"

int main_thread()
{
	while (true) {
		init_bypass();
	}

	if (!memory.setup())
	{
		silent_fail("failed to initialize memory.")
	}

	uintptr_t game_assembly = memory.module_base("GameAssembly.dll");

	if (!game_assembly)
	{
		silent_fail("failed to get gameassembly.")
	}
	else
	{
		memory.game_assembly = game_assembly;
	}


	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(entity_thread), NULL, NULL, NULL);
	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(aimbot_thread), NULL, NULL, NULL);
	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(feature_thread), NULL, NULL, NULL);

	CreateDirectoryA("C:\\rustcheat\\", NULL);
	CreateDirectoryA("C:\\rustcheat\\configs", NULL);
	CreateDirectoryA("C:\\rustcheat\\configs\\rust\\", NULL);

	config.refresh_configs();

	ImRenderer = new GRenderer();

	c_overlay->InitWindows(memory.rust_window);

	SetWindowLongA(c_overlay->GetLocalHwnd(), GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

	RECT OldRc{};
	MSG msg{};

	Beep(360, 1000);

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	while (msg.message != WM_QUIT)
	{

		if (config.wants_unload)
		{

			Beep(700, 1000);
			Sleep(1000);
			exit(0);

		}

		if (!FindWindowA(NULL, "Rust"))
			exit(0);

		if (GetAsyncKeyState(VK_INSERT) & 1)
			menu.opened = !menu.opened;

		if (PeekMessageA(&msg, c_overlay->GetLocalHwnd(), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		HWND ForegaundWind = GetForegroundWindow();
		if (memory.rust_window == ForegaundWind)
		{
			HWND GetWind = GetWindow(ForegaundWind, GW_HWNDPREV);
			SetWindowPos(c_overlay->GetLocalHwnd(), GetWind, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT Rect{};
		POINT Cord{};
		GetClientRect(memory.rust_window, &Rect);
		ClientToScreen(memory.rust_window, &Cord);
		Rect.left = Cord.x;
		Rect.top = Cord.y;

		if (Rect.left != OldRc.left || Rect.right != OldRc.right || Rect.top != OldRc.top || Rect.bottom != OldRc.bottom)
		{
			OldRc = Rect;
			c_overlay->m_pWidth = (float)Rect.right;
			c_overlay->m_pHeight = (float)Rect.bottom;
			SetWindowPos(c_overlay->GetLocalHwnd(), 0, Cord.x, Cord.y, int(Rect.right + 2.f), int(Rect.bottom + 2.f), SWP_NOREDRAW);
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_pd3dDevice->Reset(&g_d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (!menu.justther)
			D3DXCreateTextureFromFileInMemoryEx(g_pd3dDevice, &justther1, sizeof(justther1), 62, 62, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &menu.justther);

		uintptr_t camera_obj = camera.get_object();

		vmatrix_t matrix = memory.read<vmatrix_t>(camera_obj + 0x2E4);
		Vector pos = memory.read<Vector>(camera_obj + 0x42C);

		camera.set_vmatrix(matrix);
		camera.set_position(pos);

		if (menu.opened)
		{
			if (clickable != true)
				change_click(true);

			menu.render();

		}
		else
		{
			if (clickable != false)
				change_click(false);
		}

		config.freecam_bind.update();
		config.aimbot_bind.update();
		config.thirdperson_bind.update();

		config.manipulation_left_bind.update();
		config.manipulation_right_bind.update();
		config.manipulation_up_bind.update();

		config.flyhack_bind.update();

		esp_features.run(ImGui::GetBackgroundDrawList());

		static ImVec2 display_size = ImGui::GetIO().DisplaySize;

		if (config.draw_aimbot_fov)
			ImRenderer->DrawCircle(ImVec2(display_size.x / 2, display_size.y / 2), config.aimbot_fov, ImColor(255, 255, 255, 175));

		if (config.show_watermark)
		{

			std::stringstream ss;

			ss << "rustcheat.cc / ";
			ss << "Registered to: " << config.cheat_username;
			ss << " / Overlay FPS: " << static_cast<int>(ImGui::GetIO().Framerate);

			ImGui::SetNextWindowPos(ImVec2(5, 5));
			ImGui::SetNextWindowSize(ImVec2(ImGui::CalcTextSize(ss.str().c_str()).x + 15, 37));
			ImGui::Begin("##watermark", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

			ImGui::Text(ss.str().c_str());

			ImGui::End();

		}

		ImGui::Begin("##flyhackbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_UnsavedDocument | ImGuiWindowFlags_NoSavedSettings);
		{

			auto draw = ImGui::GetWindowDrawList();
			auto pos = ImGui::GetWindowPos();
			auto size = ImGui::GetWindowSize();

			ImGui::SetWindowSize(ImVec2(160, 100));

			ImVec2 title_size = menu.bold->CalcTextSizeA(15.0f, FLT_MAX, 0.0f, "Flyhack Percentage");
			draw->AddText(menu.bold, 15.0f, ImVec2(pos.x + size.x / 2 - title_size.x / 2, pos.y + 5), ImColor(255, 255, 255), "Flyhack Percentage");
			draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + 25), ImVec2(pos.x + size.x - 4, pos.y + 27), ImColor(141, 168, 241), ImColor(141, 168, 241, 0), ImColor(141, 168, 241, 0), ImColor(141, 168, 241));

			ImGui::SetCursorPosY(32);

			char x_string[40];
			sprintf_s(x_string, "%.1f", flyhack_features.current_flyhack_percentage.x * 100.f);
			
			char y_string[40];
			sprintf_s(y_string, "%.1f", flyhack_features.current_flyhack_percentage.y * 100.f);

			ImGui::Text("X Percentage: %s%%", std::string(x_string).c_str());
			ImGui::Text("Y Percentage: %s%%", std::string(y_string).c_str());

		}
		ImGui::End();

		if (config.show_keybinds)
		{

			ImGui::Begin("##keylist", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_UnsavedDocument | ImGuiWindowFlags_NoSavedSettings);
			{

				std::vector<std::string> keys;

				auto draw = ImGui::GetWindowDrawList();
				auto pos = ImGui::GetWindowPos();
				auto size = ImGui::GetWindowSize();

				if (config.freecam_bind.enabled)
				{
					std::stringstream ss;
					ss << config.freecam_bind.get_name() << " / " << config.freecam_bind.get_type();
					keys.push_back(ss.str());
				}

				if (config.aimbot_bind.enabled)
				{
					std::stringstream ss;
					ss << config.aimbot_bind.get_name() << " / " << config.aimbot_bind.get_type();
					keys.push_back(ss.str());
				}

				if (config.thirdperson_bind.enabled)
				{
					std::stringstream ss;
					ss << config.thirdperson_bind.get_name() << " / " << config.thirdperson_bind.get_type();
					keys.push_back(ss.str());
				}

				if (config.manipulation_left_bind.enabled)
				{
					std::stringstream ss;
					ss << config.manipulation_left_bind.get_name() << " / " << config.manipulation_left_bind.get_type();
					keys.push_back(ss.str());
				}

				if (config.manipulation_right_bind.enabled)
				{
					std::stringstream ss;
					ss << config.manipulation_right_bind.get_name() << " / " << config.manipulation_right_bind.get_type();
					keys.push_back(ss.str());
				}

				if (config.manipulation_up_bind.enabled)
				{
					std::stringstream ss;
					ss << config.manipulation_up_bind.get_name() << " / " << config.manipulation_up_bind.get_type();
					keys.push_back(ss.str());
				}

				if (config.flyhack_bind.enabled)
				{
					std::stringstream ss;
					ss << config.flyhack_bind.get_name() << " / " << config.flyhack_bind.get_type();
					keys.push_back(ss.str());
				}

				static int current_size; int max_size; max_size = 46 + 17 * keys.size();

				if (current_size < max_size)
					current_size++;
				else if (current_size > max_size)
					current_size--;

				ImGui::SetWindowSize(ImVec2(160, current_size));

				ImVec2 title_size = menu.bold->CalcTextSizeA(15.0f, FLT_MAX, 0.0f, "Keybinds List");
				draw->AddText(menu.bold, 15.0f, ImVec2(pos.x + size.x / 2 - title_size.x / 2, pos.y + 5), ImColor(255, 255, 255), "Keybinds List");
				draw->AddRectFilledMultiColor(ImVec2(pos.x, pos.y + 25), ImVec2(pos.x + size.x - 4, pos.y + 27), ImColor(141, 168, 241), ImColor(141, 168, 241, 0), ImColor(141, 168, 241, 0), ImColor(141, 168, 241));

				ImGui::SetCursorPosY(32);
				ImGui::BeginGroup();

				for (int i = 0; i < keys.size(); i++)
					ImGui::Text(keys.at(i).c_str());

				ImGui::EndGroup();

			}
			ImGui::End();

		}

		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_pd3dDevice->Reset(&g_d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}

	}

	return 1;

}

#ifndef _DEBUG

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	injection_data* data = reinterpret_cast<injection_data*>(lpvReserved);

	if (!lpvReserved)
		reinterpret_cast<int>(nullptr)();

	if (!data)
		reinterpret_cast<int>(nullptr)();

	if (data->size != 0x4500)
		reinterpret_cast<int>(nullptr)();

	if (strcmp(data->process_name, "ctfmon.exe"))
		reinterpret_cast<int>(nullptr)();

	config.cheat_username = data->username;
	config.cheat_time = data->expiry;

	if (data->key != 0x12102007)
		reinterpret_cast<int>(nullptr)();

	DisableThreadLibraryCalls(hinstDLL);
	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(main_thread), NULL, NULL, NULL);

	return TRUE;

}

#else

int main()
{
	main_thread();
}

#endif