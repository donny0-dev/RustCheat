#pragma once

#include "imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "ImGui/imgui_impl_win32.h"
#include "merge/byte.h"
#include "ImGui/imgui_freetype.h"
#include "merge/custom.h"
#include "c_config.h"
#include <string>

#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

enum TABS
{
    TAB_AIMBOT,
    TAB_VISUALS,
    TAB_MISC,
    TAB_CONFIG
};

#define ALPHA (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar)
#define NO_ALPHA (ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar)

#define START_LEFT_CHILD ImGui::SetCursorPos({ 191, 11 });
#define START_RIGHT_CHILD ImGui::SetCursorPos({ 424, 11 });

#define SECOND_COLORPICKER ImGui::SameLine(186 - (8 + 23));

#define OFFSET_LEFT_CHILD(x) ImGui::SetCursorPos({ 191, 19+x });
#define OFFSET_RIGHT_CHILD(x) ImGui::SetCursorPos({ 424, 19+x });

#define MAX_CHILD_SIZE ImVec2(457, 493)
#define CHILD_X_SIZE 224

class c_menu
{
public:

    ImFont* combo_arrow;
    ImFont* medium_bigger;
    ImFont* weapons;
    ImFont* medium;
    ImFont* bold;
    TABS tab = TAB_VISUALS;

    IDirect3DTexture9* justther = nullptr;

	float anim_speed = 0.05f, m_alpha = 1.f;

    bool opened = false;

	void init()
	{
		
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        ImFontConfig font_config;
        font_config.Density = ImGui_ImplWin32_GetDpiScaleForHwnd(GetCurrentProcess());
        //font_config.OversampleH = 1;
        //font_config.OversampleV = 1;
        font_config.FontBuilderFlags = ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
            0x2DE0, 0x2DFF, // Cyrillic Extended-A
            0xA640, 0xA69F, // Cyrillic Extended-B
            0xE000, 0xE226, // icons
            0,
        };

        font_config.GlyphRanges = ranges;

        medium = io.Fonts->AddFontFromMemoryTTF(PTRootUIMedium, sizeof(PTRootUIMedium), 15.0f, &font_config, ranges);
        medium_bigger = io.Fonts->AddFontFromMemoryTTF(PTRootUIMedium, sizeof(PTRootUIMedium), 16.0f, &font_config, ranges);

        bold = io.Fonts->AddFontFromMemoryTTF(PTRootUIBold, sizeof(PTRootUIBold), 15.0f, &font_config, ranges);

        weapons = io.Fonts->AddFontFromMemoryTTF(astriumwep, sizeof(astriumwep), 12.0f, &font_config, ranges);

        combo_arrow = io.Fonts->AddFontFromMemoryTTF(combo, sizeof(combo), 9.0f, &font_config, ranges);

	}

	void render()
	{

        ImGui::SetNextWindowSize({ 659, 515 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::Begin("remade.cc", nullptr, ImGuiWindowFlags_NoDecoration);

        auto draw = ImGui::GetWindowDrawList();

        auto pos = ImGui::GetWindowPos();
        auto size = ImGui::GetWindowSize();

        ImGuiStyle style = ImGui::GetStyle();

        draw->AddRectFilled(ImVec2(pos.x + 1, pos.y + 1), ImVec2(pos.x + 181, pos.y + size.y - 1), ImColor(22, 22, 22), style.WindowRounding, ImDrawFlags_RoundCornersLeft);
        draw->AddLine(ImVec2(pos.x + 181, pos.y + 1), ImVec2(pos.x + 181, pos.y + size.y - 1), ImColor(1.0f, 1.0f, 1.0f, 0.03f));

        draw->AddLine(ImVec2(pos.x + 9, pos.y + 44), ImVec2(pos.x + 172, pos.y + 44), ImColor(1.0f, 1.0f, 1.0f, 0.03f));

        draw->AddRect(pos + ImVec2(1, 1), pos + size - ImVec2(1, 1), ImColor(1.0f, 1.0f, 1.0f, 0.03f), style.WindowRounding);

        draw->AddImage(reinterpret_cast<void*>(justther), ImVec2(pos.x + 59, pos.y), ImVec2(pos.x + 121, pos.y + 54));
 
        ImGui::SetCursorPos({ 80, 11 }); {
            if (ImGui::InvisibleButton("##logomenu", ImVec2(18, 23)))
                ImGui::OpenPopup("##logo_menu");

            if (ImGui::BeginPopupModal("##logo_menu", nullptr, ImGuiWindowFlags_NoDecoration)) {
                ImGui::SetCursorPos({ 25, 15 }); {
                    ImGui::BeginChild("##container", ImVec2(265, 155)); {
                        ImGui::Text("Username:"); ImGui::SameLine(205); ImGui::TextColored(ImVec4(ImColor(141, 168, 241)), config.cheat_username.c_str());
                        ImGui::Text("Sub expires:"); ImGui::SameLine(192); ImGui::TextColored(ImVec4(ImColor(141, 168, 241)), config.cheat_time.c_str());
                        ImGui::SliderFloat("Animation speed", &anim_speed, 0.05f, 1.0f, "%.3f", ImGuiSliderFlags_NoInput);
                    } ImGui::EndChild();
                }

                ImGui::SetCursorPos({ 207, 160 }); {
                    if (ImGui::Button("Apply", ImVec2(75, 30))) ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        draw->AddText(bold, 15.0f, ImVec2(pos.x + 20, pos.y + 52), ImColor(1.0f, 1.0f, 1.0f, 0.33f), "Tabs");

        ImGui::SetCursorPos({ 9, 75 });
        ImGui::BeginGroup();
            if (elements::tab("Weapon", tab == TAB_AIMBOT)) { tab = TAB_AIMBOT; m_alpha = 0.0f; }
            if (elements::tab("Visuals", tab == TAB_VISUALS)) { tab = TAB_VISUALS; m_alpha = 0.0f; }
            if (elements::tab("Misc", tab == TAB_MISC)) { tab = TAB_MISC; m_alpha = 0.0f; }
            if (elements::tab("Configs", tab == TAB_CONFIG)) { tab = TAB_CONFIG; m_alpha = 0.0f; }
        ImGui::EndGroup();
            
        m_alpha = ImLerp(m_alpha, m_alpha < 1.0f ? 1.0f : 1.0f, anim_speed * (1.0f - ImGui::GetIO().DeltaTime));

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);

        switch (tab)
        {

            case TAB_AIMBOT:
            {

                START_LEFT_CHILD

                elements::begin_child("Aimbot", ImVec2(CHILD_X_SIZE, MAX_CHILD_SIZE.y));
                
                ImGui::Checkbox("Enable Aimbot", &config.enable_aimbot);
                ImGui::Text("Aimbot Bind");
                ImGui::SameLine();
                elements::key_bind(&config.aimbot_bind, ImVec2(57, 16));
                ImGui::SliderFloat("Aimbot FOV", &config.aimbot_fov, 1.f, 360.f, "%.0f");

                ImGui::Checkbox("Enable Smoothing", &config.aimbot_smoothing_enable);

                if (config.aimbot_smoothing_enable)
                    ImGui::SliderFloat("Aimbot Smoothing", &config.aimbot_smoothing, 1.f, 100.f, "%.0f Percent");

                ImGui::Checkbox("Enable Silent Aimbot", &config.aimbot_silent);
                ImGui::Checkbox("Enable Aimbot Prediction", &config.aimbot_prediction);

                if (config.aimbot_prediction)
                    ImGui::Combo("Aimbot Prediction Type", &config.prediction_type, "Static\0Dynamic");

                ImGui::Combo("Aimbot Target", &config.aimbot_bone_target, "Head\0Body\0Nearest");

                ImGui::Checkbox("Enable FOV Drawing", &config.draw_aimbot_fov);

                elements::end_child();

                START_RIGHT_CHILD

                elements::begin_child("Weapon Modifications", ImVec2(CHILD_X_SIZE, MAX_CHILD_SIZE.y));

                ImGui::Checkbox("Enable No-Recoil", &config.no_recoil);

                //if (config.no_recoil)
                //    ImGui::SliderFloat("Amount", &config.no_recoil_amount, 0.f, 100.f, "%.0f Percent");

                ImGui::Checkbox("Enable No-Spread", &config.no_spread);
                ImGui::Checkbox("Enable No-Sway", &config.no_sway);

                ImGui::Checkbox("Enable Auto Reload On Snipers", &config.auto_reload_l96);
                ImGui::Checkbox("Enable Stay Scoped On Reload", &config.scope_and_reload_snipers);

                ImGui::Checkbox("Enable Automatic Guns", &config.all_guns_automatic);

                ImGui::Checkbox("Enable Thick Bullet", &config.thick_bullet);
                ImGui::SliderFloat("Thick bullet thickness", &config.thickness, 0.2f, 1.f);

                ImGui::Checkbox("Enable Instant Eoka", &config.instant_eoka);
                ImGui::Checkbox("Rapidfire", &config.rapid_fire);
                ImGui::Checkbox("Instant Compound Bow", &config.instant_compound);

                elements::end_child();

                break;

            }

            case TAB_VISUALS:
            {

                START_LEFT_CHILD

                elements::begin_child("ESP", ImVec2(CHILD_X_SIZE, MAX_CHILD_SIZE.y));

                ImGui::Checkbox("Enable Box ESP", &config.show_box_esp);
                ImGui::SameLine(167);
                ImGui::ColorEdit4("Vis Box Color", &config.box_esp_color_visible.r, ImGuiColorEditFlags_AlphaBar);
                ImGui::SameLine();
                ImGui::ColorEdit4("InVis Box Color", &config.box_esp_color_invisible.r, ImGuiColorEditFlags_AlphaBar);

                ImGui::Checkbox("Enable Health ESP", &config.show_healthbar);

                ImGui::Checkbox("Enable Name ESP", &config.show_nametag);
                ImGui::SameLine(186);
                ImGui::ColorEdit4("Name Color", &config.name_esp_color_visible.r, ImGuiColorEditFlags_AlphaBar);

                ImGui::Checkbox("Enable Item ESP", &config.show_current_item);
                ImGui::SameLine(186);
                ImGui::ColorEdit4("Item Color", &config.item_esp_color_visible.r, ImGuiColorEditFlags_AlphaBar);

                elements::MultiCombo("Player ESP Filters", config.esp_filters);
                elements::MultiCombo("Player Flags", config.player_flags);
                elements::MultiCombo("Other ESP", config.extra_esp);

                ImGui::Checkbox("Enable Visual Hotbar", &config.visual_hotbar);

                elements::end_child();

                START_RIGHT_CHILD

                elements::begin_child("World Modification", ImVec2(CHILD_X_SIZE, MAX_CHILD_SIZE.y));

                ImGui::Text("Custom Sky Color");
                ImGui::SameLine(186);
                ImGui::ColorEdit4("Sky Color", &config.sky_color.r);

                ImGui::Checkbox("Always Day", &config.always_day);
                ImGui::SliderFloat("Cloud Opacity", &config.cloud_size, 1.f, 650.f, "%.1f");

                elements::end_child();

                break;

            }

            case TAB_MISC:
            {

                START_LEFT_CHILD

                elements::begin_child("Movement", ImVec2(CHILD_X_SIZE, MAX_CHILD_SIZE.y));
               
                ImGui::Checkbox("Enable Spiderman", &config.spiderman);
                ImGui::Checkbox("No Fall Damage", &config.no_fall_damage);
                ImGui::Checkbox("Enable Freecam", &config.flyhack);
                ImGui::Text("Freecam Bind");
                ImGui::SameLine();
                elements::key_bind(&config.freecam_bind, ImVec2(57, 16));
                ImGui::SliderFloat("Freecam Speed", &config.flyhack_speed, 1.f, 100.f);
                ImGui::Checkbox("Shoot In Air", &config.shootinair);
                ImGui::Checkbox("Scope With Heavy Armor", &config.scope_with_heavy);
                ImGui::Checkbox("No Heavy Movement Penalty", &config.move_full_speed_with_heavy);
                ImGui::Checkbox("Enable Flyhack", &config.enable_flyhack);
                ImGui::SameLine();
                elements::key_bind(&config.flyhack_bind, ImVec2(57, 16));

                elements::end_child();

                START_RIGHT_CHILD

                elements::begin_child("Miscellaneous", ImVec2(CHILD_X_SIZE, MAX_CHILD_SIZE.y));

                ImGui::SliderFloat("FOV Changer", &config.fov_changer, 30.f, 150.f, "%.0f");
                ImGui::Checkbox("Instant Revive", &config.instant_revive);
                ImGui::Checkbox("Shoot While Mounted", &config.shoot_in_minicopter);
                ImGui::Checkbox("Instant Weapon Deploy", &config.instant_deploy);
                ImGui::Checkbox("Enable Thirdperson", &config.thirdperson);
                ImGui::Text("Thirdperson Bind");
                ImGui::SameLine();
                elements::key_bind(&config.thirdperson_bind, ImVec2(57, 16));
                ImGui::Checkbox("Show Watermark", &config.show_watermark);
                ImGui::Checkbox("Show Keybinds List", &config.show_keybinds);

                ImGui::Checkbox("Enable Manipulation", &config.enable_manipulation);
                ImGui::Text("Manipulation Right Bind");
                ImGui::SameLine();
                elements::key_bind(&config.manipulation_right_bind, ImVec2(57, 16));

                ImGui::Text("Manipulation Left Bind");
                ImGui::SameLine();
                elements::key_bind(&config.manipulation_left_bind, ImVec2(57, 16));

                ImGui::Text("Manipulation Up Bind");
                ImGui::SameLine();
                elements::key_bind(&config.manipulation_up_bind, ImVec2(57, 16));

                if (ImGui::Button("Unload Cheat"))
                    config.wants_unload = true;

                elements::end_child();

                break;

            }

            case TAB_CONFIG:
            {

                START_LEFT_CHILD

                elements::begin_child("Configs", ImVec2(CHILD_X_SIZE, MAX_CHILD_SIZE.y));

                static int selected_config = 0;

                ImGui::SetNextItemWidth(196);
                elements::ListBox("##Configs", &selected_config, config.configs);

                elements::end_child();

                START_RIGHT_CHILD

                elements::begin_child("Config Manipulation", ImVec2(CHILD_X_SIZE, MAX_CHILD_SIZE.y));

                static char config_name[MAX_PATH];

                ImGui::SetNextItemWidth(196);
                ImGui::InputText("Config Name", config_name, sizeof(config_name));

                if (ImGui::Button("Refresh Configs", ImVec2(196, 37)))
                    config.refresh_configs();

                if (ImGui::Button("Load Config", ImVec2(196, 37)))
                    config.load_config(selected_config);

                if (ImGui::Button("Save Config", ImVec2(196, 37)))
                    config.save_config(selected_config);

                if (ImGui::Button("Create Config", ImVec2(196, 37)))
                    config.create_config(config_name);

                if (ImGui::Button("Delete Config", ImVec2(196, 37)))
                    config.delete_config(selected_config);

                elements::end_child();

                break;

            }

        }

        ImGui::PopStyleVar(2);

        ImGui::End();

	}

}; inline c_menu menu;