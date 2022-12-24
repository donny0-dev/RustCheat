#pragma once

#include "ImGui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui_internal.h"

#include "c_game.h"
#include "c_config.h"
#include "ImGuiRender/ImGuiRender.h"
#include "types/vector3.h"
#include "types/vector2.h"
#include "c_aimbot.h"
#include "c_texture_cache.h"

class c_flyhack
{
private:
	Vector offset{0.f, 1.5f, 0.f};
	UINT_PTR PlayerEyes;
	UINT_PTR PlayerEyes_Static_Objects;

	void GetPlayerEyes()
	{
		while (!PlayerEyes)
		{
			PlayerEyes = memory.read<UINT_PTR>(memory.game_assembly + 56726328);//PlayerEyes_TypeInfo 
			if (!PlayerEyes) { Sleep(100); continue; }
		}

		while (!PlayerEyes_Static_Objects)
		{
			PlayerEyes_Static_Objects = memory.read<UINT_PTR>(PlayerEyes + 0xB8);
			if (!PlayerEyes_Static_Objects) { Sleep(100); continue; }
		}
	}

	Vector old_pos = Vector(0.f, 0.f, 0.f);

	Vector2D flyhack_percentage(Vector new_pos)
	{

		if (old_pos.IsZero())
			old_pos = new_pos;

		bool flag = false;
		Vector vector4 = new_pos - entitylist.local_player->get_last_tick()->get_position();
		Vector2D per(0.f, 0.f);
		float num2 = abs(vector4.y);
		float num3 = vector4.xy_magnitude();
		if (vector4.y >= 0.f)
		{
			per.y += vector4.y;
			flag = true;
		}
		if (num2 < num3)
		{
			per.x += num3;
			flag = true;
		}
		if (flag)
		{
			float num4 = max(1.5f, 0.f);
			float num5 = 1.5 + num4;

			per.y = min(1.f, per.y / num5);

			float num6 = max(1.5f, 0.f);
			float num7 = 5.f + num6;

			per.x = min(1.f, per.x / num7);

		}

		return per;

	}

public:

	Vector2D current_flyhack_percentage = Vector2D(0.f, 0.f);

	void run_manipulation()
	{

		bool _enabled = config.enable_manipulation;

		bool binds_enabled = config.manipulation_left_bind.enabled || config.manipulation_right_bind.enabled || config.manipulation_up_bind.enabled;

		bool enabled = _enabled && binds_enabled;

		if (!enabled && offset != Vector{ 0.f, 1.5f, 0.f })
		{

			offset.x = 0.f;
			offset.y = 1.5f;
			offset.z = 0.f;

			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);

			return;

		}
		else if (!enabled)
			return;

		GetPlayerEyes();

		if (!PlayerEyes_Static_Objects || !PlayerEyes)
			return;

		if (config.manipulation_right_bind.enabled)
		{

			offset.x = 1.4f;
			offset.y = 1.5f;
			offset.z = 0.f;
			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);

		}

		if (config.manipulation_left_bind.enabled)
		{

			offset.x = -1.4f;
			offset.y = 1.5f;
			offset.z = 0.f;
			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);

		}

		if (config.manipulation_up_bind.enabled)
		{

			offset.x = 0.f;
			offset.y = 2.4f;
			offset.z = 0.f;
			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);

		}

	}

	void run_flyhack()
	{

		current_flyhack_percentage = Vector2D(0.f, 0.f);

		game_classes::playermodel* model = entitylist.local_player->get_model();

		if (!model)
			return;

		game_classes::basemovement* movement = entitylist.local_player->get_movement();

		if (!movement)
			return;

		bool enabled = config.flyhack_bind.enabled && config.enable_flyhack;
		bool flying = movement->get_flying();

		if (!enabled && flying)
			movement->set_flying(false);
		else if (!enabled)
			return;

		movement->set_flying(true);

		current_flyhack_percentage = flyhack_percentage(model->get_position());

		Vector targetmovement(0.f, 0.f, 0.f);
		game_classes::eyes* eyes = entitylist.local_player->get_eyes();

		if (!eyes)
			return;

		Vector4D rotation = eyes->get_body_rotation();

		if (!rotation.IsValid())
			return;

		const Vector vec_forward(0.f, 0.f, 1.f);
		const Vector vec_right(1.f, 0.f, 0.f);

		if (GetAsyncKeyState(VK_UP))
			targetmovement += quatmult(&vec_forward, &rotation);
		if (GetAsyncKeyState(VK_DOWN))
			targetmovement -= quatmult(&vec_forward, &rotation);
		if (GetAsyncKeyState(VK_LEFT))
			targetmovement -= quatmult(&vec_right, &rotation);
		if (GetAsyncKeyState(VK_RIGHT))
			targetmovement += quatmult(&vec_right, &rotation);

		if (GetAsyncKeyState(VK_SPACE))
			targetmovement.y += 1.f;

		float d = 2.f;
		if (GetAsyncKeyState(VK_CONTROL))
			d = 1.f;
		if (GetAsyncKeyState(VK_SHIFT))
			d = 5.f;

		//movement->set_target_movement(targetmovement * d);

		targetmovement *= d;

		if (targetmovement != Vector(0.f, 0.f, 0.f))
		{

			movement->set_target_movement(targetmovement * d);

		}

	}

	void run_freecam()
	{

		bool enabled = config.flyhack && config.freecam_bind.enabled;

		if (!enabled && offset != Vector{ 0.f, 1.5f, 0.f })
		{

			offset.x = 0.f;
			offset.y = 1.5f;
			offset.z = 0.f;

			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);

			return;

		}
		else if (!enabled)
			return;

		GetPlayerEyes();

		if (!PlayerEyes_Static_Objects || !PlayerEyes)
			return;

		float flyspeed = config.flyhack_speed / 100.f;

		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			offset.z += flyspeed;
			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			offset.z -= flyspeed;
			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			offset.x += flyspeed;
			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);
		}

		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			offset.x -= flyspeed;
			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);
		}

		if (GetAsyncKeyState(VK_RSHIFT) & 0x8000)
		{
			offset.y += flyspeed;
			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);
		}

		if (GetAsyncKeyState(VK_RCONTROL) & 0x8000)
		{
			offset.y -= flyspeed;
			memory.write<Vector>(PlayerEyes_Static_Objects + 0x0, offset);
		}

	}
}; inline c_flyhack flyhack_features;

class c_esp
{
private:
	void __fastcall DrawBox(ImVec2 pos, ImVec2 size, ImColor color)
	{
		const ImRect rect_bb(pos, pos + size);

		ImGui::GetBackgroundDrawList()->AddRect(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), ImColor(0, 0, 0), 0.0f, 15, 1.f);
		ImGui::GetBackgroundDrawList()->AddRect(rect_bb.Min + ImVec2(1, 1), rect_bb.Max - ImVec2(1, 1), ImColor(0, 0, 0), 0.0f, 15, 1.f);
		ImGui::GetBackgroundDrawList()->AddRect(rect_bb.Min, rect_bb.Max, color, 0.0f, 15, 1.f);
	}

	void __fastcall DrawHealth(int health, ImVec2 pos, ImVec2 size)
	{
		//ImColor color;

		//if (health > 95)
			//color = ImColor(0, 255, 0);
		//else if (health < 75 && health > 50)
			//color = ImColor(255, 135, 0);
		//else if (health < 50 && health > 35)
			//color = ImColor(255, 180, 0);
		//else if (health < 35 && health > 0)
			//color = ImColor(255, 0, 0);

		//ImRenderer->DrawFilledRect(ImVec2(pos.x - 5, pos.y), ImVec2(4, size.x), ImColor(0, 0, 0, int(130 * 1.f)));
		//ImRenderer->DrawFilledRect(ImVec2(pos.x - 4, pos.y), ImVec2(3, size.x), color);
	}
	std::vector<std::string> ore_names = {

	"assets/bundled/prefabs/autospawn/resource/ores/metal-ore prefab",
	"assets/bundled/prefabs/autospawn/resource/ores/stone-ore prefab",
	"assets/bundled/prefabs/autospawn/resource/ores/sulfur-ore prefab",
	"assets/bundled/prefabs/autospawn/resource/ores_sand/sulfur-ore prefab"

	};

	std::vector<std::string> short_ore_names = {

		"metal ore",
		"stone ore",
		"sulfur ore",
		"sulfure ore"

	};
public:
	void run(ImDrawList* draw)
	{

		for (auto& entity : entitylist.entities)
		{

			if (entity.name != "invalid" && entity.tag != unity_enums::object_tags::players)
			{

				if (!entity.name.empty() && entity.name.size() && !(entity.name.size() > 1024))
				{
					if (entity.name.find("guntrap deployed") != std::string::npos && config.extra_esp[4].enabled)
					{

						entity.UpdatePosition();
						Vector2D pos;
						if (world_to_screen(entity.Position, pos))
							ImRenderer->DrawTextGui("shotgun trap", ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

						continue;

					}

					if (entity.name.find("autoturret_deployed") != std::string::npos && config.extra_esp[5].enabled)
					{

						entity.UpdatePosition();
						Vector2D pos;
						if (world_to_screen(entity.Position, pos))
							ImRenderer->DrawTextGui("auto turret", ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

						continue;

					}

					if (entity.name.find("flameturret deployed") != std::string::npos && config.extra_esp[6].enabled)
					{

						entity.UpdatePosition();
						Vector2D pos;
						if (world_to_screen(entity.Position, pos))
							ImRenderer->DrawTextGui("flame turret", ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

						continue;

					}

					if (entity.name.find("beartrap") != std::string::npos && config.extra_esp[7].enabled)
					{

						entity.UpdatePosition();
						Vector2D pos;
						if (world_to_screen(entity.Position, pos))
							ImRenderer->DrawTextGui("bear trap", ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

						continue;

					}

					if (entity.name.find("sam_site_turret_deployed") != std::string::npos && config.extra_esp[8].enabled)
					{

						entity.UpdatePosition();
						Vector2D pos;
						if (world_to_screen(entity.Position, pos))
							ImRenderer->DrawTextGui("sam site", ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

						continue;

					}

					if (entity.name.find("landmine") != std::string::npos && config.extra_esp[9].enabled)
					{

						entity.UpdatePosition();
						Vector2D pos;
						if (world_to_screen(entity.Position, pos))
							ImRenderer->DrawTextGui("land mine", ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

						continue;

					}

					if (entity.name.find("stash") != std::string::npos && config.extra_esp[0].enabled)
					{

						entity.UpdatePosition();
						Vector2D pos;
						if (world_to_screen(entity.Position, pos))
							ImRenderer->DrawTextGui("stash", ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

						continue;

					}

					if (entity.name.find("backpack") != std::string::npos && config.extra_esp[1].enabled)
					{

						entity.UpdatePosition();
						Vector2D pos;
						if (world_to_screen(entity.Position, pos))
							ImRenderer->DrawTextGui("backpack", ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

						continue;

					}

					if (entity.name.find("cupboard tool") != std::string::npos && config.extra_esp[3].enabled)
					{

						entity.UpdatePosition();
						Vector2D pos;
						if (world_to_screen(entity.Position, pos))
							ImRenderer->DrawTextGui("tool cupboard", ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

						continue;

					}

					for (int i = 0; i < ore_names.size(); i++)
						if (entity.name.find(ore_names[i]) != std::string::npos && config.extra_esp[2].enabled)
						{

							entity.UpdatePosition();
							Vector2D pos;
							if (world_to_screen(entity.Position, pos))
								ImRenderer->DrawTextGui(short_ore_names[i], ImVec2(pos.x, pos.y), 13.f, ImColor(255, 255, 255, 255), true, nullptr);

							continue;

						}

				}

			}

			if (entity.tag != unity_enums::object_tags::players)
				continue;

			game_classes::player* player = (game_classes::player*)entity.Base;

			if (!player)
				continue;

			bool teammate = player->is_same_team(entitylist.local_player);

			if (config.esp_filters[0].enabled && teammate)
				continue;

			if (player->get_flags() & (int)unity_enums::player_flags::Sleeping && config.esp_filters[3].enabled)
				continue;

			game_classes::playermodel* model = player->get_model();

			if (!model)
				continue;

			if (config.esp_filters[1].enabled && model->is_npc())
				continue;

			if (model->is_local_player())
				continue;

			entity.UpdatePosition();

			boundingbox box = player->get_bounds_box();

			if (!box.valid)
				continue;

			if (config.show_box_esp)
				DrawBox(ImVec2(box.x, box.y), ImVec2(box.w, box.h), player->is_visible() ? config.box_esp_color_visible.to_imcolor() : config.box_esp_color_invisible.to_imcolor());

			if (config.show_nametag)
			{

				std::string additive = "";
				std::string player_name = player->get_name();

				if (config.player_flags[0].enabled)
				{

					float distance = camera.get_position().distance(entity.Position);
					char tmp[40];
					sprintf_s(tmp, "%.1f", distance);
					additive = " (" + std::string(tmp) + "M)";

				}

				if (config.show_healthbar)
				{
					float health = player->get_health();

					char tmp[40];
					sprintf_s(tmp, "%1.f", health);

					if (player->get_flags() & (int)unity_enums::player_flags::Wounded)
						additive = " (DOWN)";
					else
						additive = " (" + std::string(tmp) + "HP)";

				}

				ImRenderer->DrawTextGui(player_name + additive, ImVec2(box.head_pos.x, box.head_pos.y - 35), 13.f, config.name_esp_color_visible.to_imcolor(), true, nullptr);

			}

			if (config.show_current_item)
				ImRenderer->DrawTextGui(player->get_held_item()->get_definition()->get_name(), ImVec2(box.head_pos.x, box.l_foot_pos.y), 13.f, config.item_esp_color_visible.to_imcolor(), true, nullptr);
		}

		if (config.visual_hotbar)
		{ // this lets you isolate variables, really useful imo!

			std::vector<c_aim_target> plrs;

			for (auto& entity : entitylist.entities)
			{

				if (entity.tag != unity_enums::object_tags::players)
					continue; // skip all non player entities

				Vector2D transform_screen;

				if (!world_to_screen(entity.Position, transform_screen))
					continue; // if they're not in our immediate FOV

				game_classes::player* player = (game_classes::player*)entity.Base;

				if (!player)
					continue;

				game_classes::playermodel* model = player->get_model();

				if (model->is_local_player())
					continue;

				c_aim_target tmp;
				tmp.validated = true;
				tmp.aim_point = entity.Position;

				Vector2D screen_position;

				world_to_screen(entity.Position, screen_position);

				ImVec2 display_size = ImGui::GetIO().DisplaySize;
				Vector2D screen_center(display_size.x / 2, display_size.y / 2);

				tmp.fov = screen_position.distance(screen_center);
				tmp.player = player;

				plrs.push_back(tmp);

			}

			auto sort_func = [](c_aim_target t1, c_aim_target t2) {

				return t1.fov < t2.fov;

			};

			if (plrs.size() > 0)
			{

				std::sort(plrs.begin(), plrs.end(), sort_func);

				ImGui::Begin("##hotbaresp", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

				std::stringstream ss;

				ss << plrs[0].player->get_name();

				auto __windowWidth = ImGui::GetWindowSize().x;
				auto __textWidth = ImGui::CalcTextSize(ss.str().c_str()).x;

				ImGui::SetCursorPosX((__windowWidth - __textWidth) * 0.5f);

				ImGui::Text(ss.str().c_str());

				ImGui::Separator();

				uintptr_t belt_container = plrs[0].player->get_belt();
				int active_id = plrs[0].player->get_active_id();

				int size = memory.read<int>(belt_container + 0x18);

				uintptr_t contents = memory.read<uintptr_t>(belt_container + 0x10);

				for (uintptr_t i = 0; i < size; i++)
				{

					game_classes::item* _item = memory.read<game_classes::item*>(contents + (0x20 + (i * 0x8)));

					if (!_item)
						continue;

					game_classes::item_definition* definition = _item->get_definition();

					auto texture = texture_cache.get_texture(definition->get_real_name());

					auto windowWidth = ImGui::GetWindowSize().x;
					auto textWidth = 90;

					ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);

					if (texture.texture)
						ImGui::Image(reinterpret_cast<void*>(texture.texture), ImVec2(90, 90), ImVec2(0, 0), ImVec2(1, 1), _item->get_id() == active_id ? ImVec4(147 / 255.f, 164 / 255.f, 226 / 255.f, 1.f) : ImVec4(1.f, 1.f, 1.f, 1.f), _item->get_id() == active_id ? ImVec4(147 / 255.f, 164 / 255.f, 226 / 255.f, 1.f) : ImVec4(0.f, 0.f, 0.f, 0.f));

					std::string text = "x" + std::to_string(_item->get_amount());

					auto _windowWidth = ImGui::GetWindowSize().x;
					auto _textWidth = ImGui::CalcTextSize(text.c_str()).x;

					ImGui::SetCursorPosX((_windowWidth - _textWidth) * 0.5f);

					ImGui::Text(text.c_str());

				}

				ImGui::End();

			}
		}

	}
}; inline c_esp esp_features;

class c_misc
{
private:
	UINT_PTR ConVarAdmin;
	UINT_PTR ConVarAdmin_StaticObjects;

	void GetConVarAdmin()
	{
		while (!ConVarAdmin)
		{
			ConVarAdmin = memory.read<UINT_PTR>(memory.game_assembly + 56830176);//ConVar_Admin_c 
			if (!ConVarAdmin) { Sleep(100); continue; }
		}

		while (!ConVarAdmin_StaticObjects)
		{
			ConVarAdmin_StaticObjects = memory.read<UINT_PTR>(ConVarAdmin + 0xB8);
			if (!ConVarAdmin_StaticObjects) { Sleep(100); continue; }
		}
	}
public:
	void run()
	{

		if (!entitylist.local_player)
			return;

		game_classes::item* i = entitylist.local_player->get_held_item();

		if (config.spiderman || config.no_fall_damage)
		{

			game_classes::basemovement* movement = entitylist.local_player->get_movement();

			if (movement)
			{

				if (config.spiderman)
					movement->setGroundAngleNew(0.f);

				if (config.no_fall_damage)
					movement->setGroundTime(9999999.f);

			}

		}

		if (config.instant_revive)
		{

			uintptr_t assist_player = memory.read<uintptr_t>((uintptr_t)entitylist.local_player + 0x348);

			if (assist_player)
				memory.write<float>(assist_player + 0x28, 0.01f);

		}

		bool in_thirdperson = entitylist.local_player->get_flags() & 1024;

		if ((config.thirdperson && config.thirdperson_bind.enabled) && !in_thirdperson)
			entitylist.local_player->set_flag(1024);
		else if (!(config.thirdperson && config.thirdperson_bind.enabled) && in_thirdperson)
			entitylist.local_player->remove_flag(1024);

		if (config.no_sway)
			memory.write<float>((uintptr_t)entitylist.local_player + 0x76C, 999.f);

		if (config.scope_with_heavy)
			memory.write<bool>((uintptr_t)entitylist.local_player + 0x760, false);

		if (config.move_full_speed_with_heavy)
			memory.write<float>((uintptr_t)entitylist.local_player + 0x764, 0.f);

		if ((entitylist.local_player->get_modelstate()->flags() & (int)unity_enums::model_flags::Mounted) && config.shoot_in_minicopter)
		{

			game_classes::mounted* mounted = entitylist.local_player->get_mounted();;

			if (!mounted->get_can_hold_items())
				mounted->can_hold_items(true);

		}

		if (i->is_weapon())
		{

			game_classes::base_projectile* weapon = i->projectile();

			if (weapon) // miss me with that gay shit
			{

				std::string weapon_name = i->get_definition()->get_real_name();

				if (config.no_spread)
					weapon->null_spread();

				if (config.no_recoil || config.no_spread)
				{

					game_classes::recoil_properties* recoil_properties = weapon->get_recoil();
					if (config.no_recoil)
						recoil_properties->null_recoil();

					if (config.no_spread)
						recoil_properties->set_aimcone_scale(0.f);

					game_classes::recoil_properties* override = recoil_properties->get_override();

					if (override)
					{

						if (config.no_recoil)
							override->null_recoil();

						if (config.no_spread)
							override->set_aimcone_scale(0.f);

					}

				}

				if (config.thick_bullet || config.no_spread)
				{

					uintptr_t created_projectiles = weapon->get_created_projectiles();

					uintptr_t projectile_list = memory.read<uintptr_t>(created_projectiles + 0x10);
					int projectile_list_size = memory.read<int>(created_projectiles + 0x18);

					for (int i = 0; i < projectile_list_size; i++)
					{

						game_classes::projectile* projectile = memory.read<game_classes::projectile*>(projectile_list + (0x20 + (i * 0x8)));

						if (!projectile)
							continue;

						if (config.thick_bullet)
							projectile->set_thickness(config.thickness);

						if (config.no_spread)
							projectile->get_mod()->set_spread(-1.f);

					}

				}

				if (weapon_name == "pistol.eoka" && config.instant_eoka)
				{

					if (GetAsyncKeyState(VK_LBUTTON))
					{
						memory.write<int>((uintptr_t)weapon + 0x388, 1);
						memory.write<int>((uintptr_t)weapon + 0x389, 1);
					}
					else
					{
						memory.write<int>((uintptr_t)weapon + 0x389, 0);
					}

				}

				if (weapon_name == "bow.compound")
					memory.write<float>((uintptr_t)weapon + 0x390, 0.f);
				
				if (config.all_guns_automatic)
					memory.write<bool>((uintptr_t)weapon + 0x290, true);

				if (config.auto_reload_l96)
					memory.write<bool>((uintptr_t)weapon + 0x30E, false);

				if (config.scope_and_reload_snipers)
					memory.write<bool>((uintptr_t)weapon + 0x30D, false);

				if (config.rapid_fire)
					memory.write<float>((uintptr_t)weapon + 0x1FC, 0.1f);

				if (config.instant_deploy)
					memory.write<float>((uintptr_t)weapon + 0x25C, 10.11f);

			}
		}

		if (config.always_day)
		{

			memory.write<float>(ConVarAdmin_StaticObjects + 0x0, 12.f);

		}
		else
		{

			memory.write<float>(ConVarAdmin_StaticObjects + 0x0, -1.f);

		}

		game_classes::inputmessage* state = entitylist.local_player->get_input()->get_inputstate()->get_current();
		Vector angles = state->get_aim_angle();

		state->set_aim_angle(Vector(angles.x, angles.y + 15, angles.z));
		

		uintptr_t tod_sky_c = memory.chain<uintptr_t>(memory.game_assembly, { 56852656, 0xB8, 0x0, 0x10, 0x20 });

		if (tod_sky_c)
		{

			uintptr_t day = memory.read<uintptr_t>(tod_sky_c + 0x50);
			uintptr_t sky_color = memory.read<uintptr_t>(day + 0x28);
			uintptr_t sky_gradient = memory.read<uintptr_t>(sky_color + 0x10);
			memory.write<Color>(sky_gradient, config.sky_color);

			uintptr_t night = memory.read<uintptr_t>(tod_sky_c + 0x58);
			uintptr_t night_sky_color = memory.read<uintptr_t>(night + 0x28);
			uintptr_t night_sky_gradient = memory.read<uintptr_t>(night_sky_color + 0x10);
			memory.write<Color>(night_sky_gradient, config.sky_color);

			uintptr_t clouds = memory.read<uintptr_t>(tod_sky_c + 0x78);
			memory.write<float>(clouds + 0x14, config.cloud_size);

		}

		static uintptr_t fov_cvar = memory.chain<uintptr_t>(memory.game_assembly, { 56843288, 0xB8 });

		if (fov_cvar)
			memory.write<float>(fov_cvar + 0x18, config.fov_changer);

		bool admincheat = memory.read<bool>((uintptr_t)entitylist.local_player->get_movement() + 0x18);

		if (config.shootinair && !admincheat)
			memory.write<bool>((uintptr_t)entitylist.local_player->get_movement() + 0x18, true);
		else if(admincheat && !config.shootinair)
			memory.write<bool>((uintptr_t)entitylist.local_player->get_movement() + 0x18, false);

	}
}; inline c_misc misc_features;