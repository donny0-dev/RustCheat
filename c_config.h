#pragma once

#include <filesystem>
#include <fstream>

#include "jsoncpp/json.h"
#include "jsoncpp/json-forwards.h"

#include "ImGui/imgui.h"
#include "c_keybind.h"

#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000

#define IM_COL32(R,G,B,A)    (((unsigned int)(A)<<IM_COL32_A_SHIFT) | ((unsigned int)(B)<<IM_COL32_B_SHIFT) | ((unsigned int)(G)<<IM_COL32_G_SHIFT) | ((unsigned int)(R)<<IM_COL32_R_SHIFT))

class Color
{
public:
	float r, g, b, a;

	Color(float _r, float _g, float _b, float _a)
	{

		r = _r;
		g = _g;
		b = _b;
		a = _a;

	}

	unsigned int to_u32()
	{

		return IM_COL32((r * 255.f), (g * 255.f), (b * 255.f), (a * 255.f));

	}

	ImColor to_imcolor()
	{
		
		return ImColor(r, g, b, a);

	}

};

__forceinline Color from_u32(unsigned int col)
{

	Color ret(((col) & 0xFF) / 255.f, ((col >> IM_COL32_G_SHIFT) & 0xFF) / 255.f, ((col >> IM_COL32_B_SHIFT) & 0xFF) / 255.f, ((col >> IM_COL32_A_SHIFT) & 0xFF) / 255.f);

	return ret;

}

struct c_config_item {
	std::string name;
	std::string path;
	int id;
};

struct c_listbox_item
{

	std::string name;
	bool enabled = false;

};

class c_config
{
public:

	std::vector<c_config_item> configs{};

	std::string cheat_username = "";
	std::string cheat_time = "";

	// aimbot

	bool enable_aimbot = false;
	float aimbot_fov = 10.f;
	int aimbot_bone_target = 1;
	float aimbot_smoothing = 10.f;
	bool aimbot_smoothing_enable = true;
	bool aimbot_silent = false;
	bool aimbot_prediction = false;
	bool draw_aimbot_fov = true;
	int prediction_type = 0;

	// visual

	bool show_box_esp = true;
	bool show_healthbar = true;
	bool show_nametag = true;
	bool show_current_item = true;
	bool visual_hotbar = true;
	bool always_day = false;

	Color box_esp_color_visible = Color(1.f, 0.f, 0.f, 1.f);
	Color box_esp_color_invisible = Color(1.f, 0.f, 0.f, 1.f);
	Color name_esp_color_visible = Color(1.f, 0.f, 0.f, 1.f);
	Color item_esp_color_visible = Color(1.f, 0.f, 0.f, 1.f);

	std::vector<c_listbox_item> esp_filters{ c_listbox_item{"Teammates", true}, c_listbox_item{"NPCs", true}, c_listbox_item{"Downed Players"}, c_listbox_item{"Sleeping Players", true} };
	std::vector<c_listbox_item> player_flags{ c_listbox_item{"Distance"}, c_listbox_item{"Health"}, c_listbox_item{"Downed"}, c_listbox_item{"Ammo"} };
	std::vector<c_listbox_item> extra_esp{ c_listbox_item{"Stashes"}, c_listbox_item{"Backpacks"}, c_listbox_item{"Ores"}, c_listbox_item{"Tool Cupboards"}, c_listbox_item{"Shotgun Traps"}, c_listbox_item{"Auto Turret"}, c_listbox_item{"Flame Turret"}, c_listbox_item{"Bear Trap"}, c_listbox_item{"SAM Site"}, c_listbox_item{"Land Mine"} };

	// misc

	bool auto_reload_l96 = false;
	bool scope_and_reload_snipers = false;
	bool shoot_in_minicopter = false;
	bool no_recoil = false;
	float no_recoil_amount = 1.f;
	bool no_spread = false;
	bool scope_with_heavy = false;
	bool no_sway = false;
	bool move_full_speed_with_heavy = false;
	bool instant_revive = false;
	float fov_changer = 90.f;
	bool spiderman = false;
	bool no_fall_damage = false;
	bool shootinair = false;
	bool flyhack = false;
	float flyhack_speed = 5.f;
	bool all_guns_automatic = false;
	bool thick_bullet = false;
	float thickness = 0.2f;
	bool instant_eoka = false;
	bool rapid_fire = false;
	bool instant_deploy = false;
	bool thirdperson = false;
	bool instant_compound = false;
	Color sky_color = Color(1.f, 1.f, 1.f, 1.f);
	float cloud_size = 5.f;
	bool show_watermark = false;
	bool show_keybinds = false;
	bool enable_manipulation = false;
	bool enable_flyhack = false;

	c_keybind freecam_bind = c_keybind("Freecam");
	c_keybind aimbot_bind = c_keybind("Aimbot");
	c_keybind thirdperson_bind = c_keybind("Thirdperson");

	c_keybind manipulation_right_bind = c_keybind("Manipulation (R)");
	c_keybind manipulation_left_bind = c_keybind("Manipulation (L)");
	c_keybind manipulation_up_bind = c_keybind("Manipulation (UP)");
	c_keybind flyhack_bind = c_keybind("Flyhack");

	bool wants_unload = false;

	void refresh_configs()
	{

		configs.clear();

		int cur_id = -1;
		std::string script_folder = "C:\\rustcheat\\configs\\rust\\";

		for (auto& entry : std::filesystem::directory_iterator(script_folder))
		{

			if (entry.path().extension() == ".json") // filters out everything that's not a lua file
			{

				std::filesystem::path file_path = entry.path();
				std::string file_name = file_path.filename().string();
				std::string file_path_string = file_path.string();
				cur_id += 1;

				c_config_item config = { file_name, file_path_string, cur_id };

				configs.emplace_back(config);

			}

		}

	}

	void load_config(int id)
	{

		if (!(id >= 0 && id < configs.size()) || id == 0 && configs.size() == 0)
			return;

		std::ifstream config_file(configs.at(id).path, std::ifstream::binary);

		if (config_file.peek() == std::ifstream::traits_type::eof())
			return;

		Json::Value config_text;
		config_file >> config_text;

		#define gb(a) config_text[a].asBool(); 
		#define gf(a) config_text[a].asFloat();
		#define gi(a) config_text[a].asInt();
		#define gc(a) from_u32(config_text[a].asUInt());

		enable_aimbot = gb("enable_aimbot")
		aimbot_fov = gf("aimbot_fov")
		aimbot_bone_target = gi("aimbot_bone_target")
		aimbot_smoothing = gf("aimbot_smoothing")
		aimbot_smoothing_enable = gb("aimbot_smoothing_enable")
		aimbot_silent = gb("aimbot_silent")
		aimbot_prediction = gb("aimbot_prediction")
		draw_aimbot_fov = gb("draw_aimbot_fov")
		prediction_type = gi("prediction_type")

		show_box_esp = gb("show_box_esp")
		show_healthbar = gb("show_healthbar")
		show_nametag = gb("show_nametag")
		show_current_item = gb("show_current_item")
		visual_hotbar = gb("visual_hotbar")
		always_day = gb("always_day")

		box_esp_color_visible = gc("box_esp_color")
		name_esp_color_visible = gc("name_esp_color")
		item_esp_color_visible = gc("item_esp_color")

		esp_filters[0].enabled = gb("teammate_filter")
		esp_filters[1].enabled = gb("npc_filter")
		esp_filters[2].enabled = gb("downed_filter")
		esp_filters[3].enabled = gb("sleeping_filter")

		player_flags[0].enabled = gb("distance_flag")
		player_flags[1].enabled = gb("health_flag")
		player_flags[2].enabled = gb("downed_flag")
		player_flags[3].enabled = gb("ammo_flag")

		extra_esp[0].enabled = gb("stash_extra")
		extra_esp[1].enabled = gb("backpack_extra")
		extra_esp[2].enabled = gb("ore_extra")
		extra_esp[3].enabled = gb("tc_extra")

		auto_reload_l96 = gb("auto_reload_sniper")
		scope_and_reload_snipers = gb("scope_and_reload_snipers")
		shoot_in_minicopter = gb("shoot_while_mounted")
		no_recoil = gb("no_recoil")
		scope_with_heavy = gb("scope_with_heavy")
		no_spread = gb("no_spread")
		no_sway = gb("no_sway")
		move_full_speed_with_heavy = gb("move_full_speed_with_heavy")
		instant_revive = gb("instant_revive")
		fov_changer = gf("fov_changer")
		spiderman = gb("spiderman")
		no_fall_damage = gb("no_fall_damage")
		shootinair = gb("shoot_in_air")
		flyhack = gb("freecam")
		flyhack_speed = gf("freecam_speed")
		all_guns_automatic = gb("all_guns_automatic")
		thick_bullet = gb("thick_bullet")
		thickness = gf("thickness")
		instant_eoka = gb("instant_eoka")
		rapid_fire = gb("rapid_fire")
		instant_deploy = gb("instant_deploy")
		thirdperson = gb("thirdperson")
		instant_compound = gb("instant_compound")
		sky_color = gc("sky_color")
		cloud_size = gf("cloud_opacity")

		aimbot_bind.key = gi("aimbot_key")
		aimbot_bind.type = (c_keybind::c_keybind_type)gi("aimbot_bind_type")
		freecam_bind.key = gi("freecam_key")
		freecam_bind.type = (c_keybind::c_keybind_type)gi("freecam_bind_type")
		thirdperson_bind.key = gi("thirdperson_key")
		thirdperson_bind.type = (c_keybind::c_keybind_type)gi("thirdperson_bind_type")

	}

	void create_config(const char* config_name)
	{

		std::string path = "C:\\rustcheat\\configs\\rust\\";
		path += config_name;
		path += ".json";

		std::ofstream file(path);
		refresh_configs();

	}

	void delete_config(int id)
	{

		if (!(id >= 0 && id < configs.size()) || id == 0 && configs.size() == 0)
			return;

		remove(configs.at(id).path.c_str());
		refresh_configs();

	}

	void save_config(int id)
	{

		if (!(id >= 0 && id < configs.size()) || id == 0 && configs.size() == 0)
			return;

		std::ofstream config_file(configs.at(id).path, std::ifstream::binary);
		Json::Value config_data;
		
		#define tf(a, v) config_data[a] = v; 

		tf("enable_aimbot", enable_aimbot);
		tf("aimbot_fov", aimbot_fov);
		tf("aimbot_bone_target", aimbot_bone_target);
		tf("aimbot_smoothing", aimbot_smoothing);
		tf("aimbot_smoothing_enable", aimbot_smoothing_enable);
		tf("aimbot_silent", aimbot_silent);
		tf("aimbot_prediction", aimbot_prediction);
		tf("draw_aimbot_fov", draw_aimbot_fov);
		tf("prediction_type", prediction_type);

		tf("show_box_esp", show_box_esp);
		tf("show_healthbar", show_healthbar);
		tf("show_nametag", show_nametag);
		tf("show_current_item", show_current_item);
		tf("visual_hotbar", visual_hotbar);
		tf("always_day", always_day);

		tf("box_esp_color", box_esp_color_visible.to_u32());
		tf("name_esp_color", name_esp_color_visible.to_u32());
		tf("item_esp_color", item_esp_color_visible.to_u32());

		tf("teammate_filter", esp_filters[0].enabled);
		tf("npc_filter", esp_filters[1].enabled);
		tf("downed_filter", esp_filters[2].enabled);
		tf("sleeping_filter", esp_filters[3].enabled);

		tf("distance_flag", player_flags[0].enabled);
		tf("health_flag", player_flags[1].enabled);
		tf("downed_flag", player_flags[2].enabled);
		tf("ammo_flag", player_flags[3].enabled);

		tf("stash_extra", extra_esp[0].enabled);
		tf("backpack_extra", extra_esp[1].enabled);
		tf("ore_extra", extra_esp[2].enabled);
		tf("tc_extra", extra_esp[3].enabled);

		tf("auto_reload_sniper", auto_reload_l96);
		tf("scope_and_reload_snipers", scope_and_reload_snipers);
		tf("shoot_while_mounted", shoot_in_minicopter);
		tf("no_recoil", no_recoil);
		tf("scope_with_heavy", scope_with_heavy);
		tf("no_sway", no_sway);
		tf("no_spread", no_spread);
		tf("move_full_speed_with_heavy", move_full_speed_with_heavy);
		tf("instant_revive", instant_revive);
		tf("fov_changer", fov_changer);
		tf("spiderman", spiderman);
		tf("no_fall_damage", no_fall_damage);
		tf("shoot_in_air", shootinair);
		tf("freecam", flyhack);
		tf("freecam_speed", flyhack_speed);
		tf("all_guns_automatic", all_guns_automatic);
		tf("thick_bullet", thick_bullet);
		tf("thickness", thickness);
		tf("instant_eoka", instant_eoka);
		tf("rapid_fire", rapid_fire);
		tf("instant_deploy", instant_deploy);
		tf("thirdperson", thirdperson);
		tf("instant_compound", instant_compound);
		tf("sky_color", sky_color.to_u32());
		tf("cloud_opacity", cloud_size);

		tf("aimbot_key", aimbot_bind.key)
		tf("freecam_key", freecam_bind.key)

		tf("aimbot_bind_type", aimbot_bind.type)
		tf("freecam_bind_type", freecam_bind.type)

		tf("thirdperson_key", thirdperson_bind.key)
		tf("thirdperson_bind_type", thirdperson_bind.type)

		config_file.clear();

		config_file << config_data;

		config_file.close();

	}

}; inline c_config config;