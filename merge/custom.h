#pragma once

#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../c_config.h"

using namespace ImGui;

namespace elements {
    bool tab(const char* name, bool boolean);
    bool subtab(const char* name, bool boolean, float width);

    bool begin_child_ex(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags);
    bool begin_child(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
    bool begin_child(ImGuiID id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
    void end_child();

    bool key_bind(c_keybind* keybind, const ImVec2& size_arg = ImVec2(0, 0), bool clicked = false, ImGuiButtonFlags flags = 0);

    bool Combo(const char* label, int* currIndex, std::vector<c_config_item>& values);
    bool ListBox(const char* label, int* currIndex, std::vector<c_config_item>& values);

    void MultiCombo(const char* label, std::vector<c_listbox_item> &items);

}