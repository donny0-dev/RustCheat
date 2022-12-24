#pragma once

#include "c_game.h"
#include "c_menu.h"
#include "types/vector2.h"

#include <cmath>
#include <algorithm>
#include <map>
#include <random>

#include <d3dx9.h>

class c_aim_target
{
public:
	game_classes::player* player;
	Vector aim_point;
	bool validated = false;
	float fov;
	float distance;
};

class c_ballistics
{
public:

	Vector draw_pos = Vector(0, 0, 0);

	int floor_to_int(float f)
	{
		return (int)floor((double)f);
	}

	void simulate_projectile(Vector& position, Vector& velocity, float& partialTime, float travelTime, Vector& gravity, float drag, Vector& prevPosition, Vector& prevVelocity)
	{
		float num = 0.03125f;
		prevPosition = position;
		prevVelocity = velocity;
		if (partialTime > 1E-45f)
		{
			float num2 = num - partialTime;
			if (travelTime < num2)
			{
				prevPosition = position;
				prevVelocity = velocity;
				position += velocity * travelTime;
				partialTime += travelTime;
				return;
			}
			prevPosition = position;
			prevVelocity = velocity;
			position += velocity * num2;
			velocity += gravity * num;
			velocity -= velocity * drag * num;
			travelTime -= num2;
		}
		int num3 = floor_to_int(travelTime / num);
		for (int i = 0; i < num3; i++)
		{
			prevPosition = position;
			prevVelocity = velocity;
			position += velocity * num;
			velocity += gravity * num;
			velocity -= velocity * drag * num;
		}
		partialTime = travelTime - num * (float)num3;
		if (partialTime > 1E-45f)
		{
			prevPosition = position;
			prevVelocity = velocity;
			position += velocity * partialTime;
		}
	}

	Vector predict_position(Vector target_position, Vector target_velocity, Vector local_position, Vector local_velocity, c_bullet_info bullet_info)
	{

		float bullet_speed = bullet_info.velocity;
		const float speedmult = 1.0f - 0.015625f * bullet_info.drag;
		bullet_speed *= speedmult;

		float distance_to_position = local_position.distance(target_position);

		auto time = (distance_to_position / bullet_speed);

		std::cout << "total time: " << time << "\n";

		Vector unity_gravity(0.f, -9.81f, 0.f);
		Vector projectile_velocity(bullet_info.velocity, bullet_info.velocity, bullet_info.velocity);

		Vector previous_position;
		Vector previous_velocity;

		float partial_time = 0.f;

		for (float ms = 0.f; ms < time; ms += 0.001f) // loop through every ms of the bullet's travel
		{

			std::cout << "current ms: " << ms << "\n";

			Vector tmp_start_pos = local_position;

			simulate_projectile(tmp_start_pos, projectile_velocity, partial_time, ms, unity_gravity, bullet_info.drag, previous_position, previous_velocity);

			std::cout << "partial time: " << partial_time << "\n";

			Vector difference = target_position - tmp_start_pos;

			std::cout << "diff x: " << difference.x << " diff y: " << difference.y << " diff z: " << difference.z << "\n";

			draw_pos = target_position;

		}

		return target_position;

	}

	std::vector<Vector> points;

	constexpr int radToDeg(float rad) { return rad * (180 / M_PI); }

	int vectorAngle(int x, int y) {
		if (x == 0) // special cases
			return (y > 0) ? 90
			: (y == 0) ? 0
			: 270;
		else if (y == 0) // special cases
			return (x >= 0) ? 0
			: 180;
		int ret = radToDeg(atanf((float)y / x));
		if (x < 0 && y < 0) // quadrant
			ret = 180 + ret;
		else if (x < 0) // quadran
			ret = 180 + ret; // it actually substracts
		else if (y < 0) // quadrant
			ret = 270 + (90 + ret); // it actually substracts
		return ret;
	}

	Vector AngleToDirection(Vector2D angle) {
		// Convert angle to radians 
		angle.x = (angle.x) * 3.14159265 / 180;
		angle.y = (angle.y) * 3.14159265 / 180;

		float sinYaw = sin(angle.y);
		float cosYaw = cos(angle.y);

		float sinPitch = sin(angle.x);
		float cosPitch = cos(angle.x);

		Vector direction;
		direction.x = cosPitch * cosYaw;
		direction.y = cosPitch * sinYaw;
		direction.z = -sinPitch;

		return direction;
	}

	bool pasted_uc_func(c_bullet_info inf, Vector start_position)
	{

		std::cout << "sz: " << points.size() << "\n";
		std::cout << "drag: " << inf.drag << " grav_mod: " << inf.gravity << " vel: " << inf.velocity << "\n";
		std::cout << "vel scale: " << inf.velocity_scale << " vel scalar: " << inf.velocity_scalar << "\n";

		points.clear();

		float Gravity = -9.81f * inf.gravity;//9.81f

		D3DXVECTOR3 StartVelocity, UnitVelocity;
		Vector tmp = AngleToDirection(entitylist.local_player->get_input()->get_viewangles());
		UnitVelocity = D3DXVECTOR3(tmp.x, tmp.y, tmp.z);

		D3DXVec3Normalize(&StartVelocity, &UnitVelocity);
		StartVelocity *= inf.velocity * inf.velocity_scale;

		D3DXVECTOR3 StartPos = D3DXVECTOR3(start_position.x, start_position.y, start_position.z);

		D3DXVECTOR3 CurrentProjectilePos = StartPos;
		D3DXVECTOR3 CurrentProjectileVecloity = StartVelocity; float OldGravity = 0;
		float TimeStep = 0.1f;
		for (float CurrentTime = 0; CurrentTime < 5.5f; CurrentTime += TimeStep)
		{
			//For gravity
			//OldGravity += Gravity * TimeStep * (1.f - TimeStep * VelocityInfo.Drag);

			CurrentProjectileVecloity.y += Gravity * TimeStep;// Gravity * TimeStep;

			CurrentProjectileVecloity.x -= (CurrentProjectileVecloity.x * TimeStep * inf.drag);
			CurrentProjectileVecloity.y -= (CurrentProjectileVecloity.y * TimeStep * inf.drag);
			CurrentProjectileVecloity.z -= (CurrentProjectileVecloity.z * TimeStep * inf.drag);

			CurrentProjectilePos += CurrentProjectileVecloity * TimeStep;

			//printf("Proj[t=%f] pos[%f %f %f] vel[%f]\n", CurrentTime, CurrentProjectilePos.x, CurrentProjectilePos.y, CurrentProjectilePos.z,
				//sqrt((CurrentProjectileVecloity.x * CurrentProjectileVecloity.x) + (CurrentProjectileVecloity.y * CurrentProjectileVecloity.y) + (CurrentProjectileVecloity.z * CurrentProjectileVecloity.z)));

			points.push_back(Vector(CurrentProjectilePos.x, CurrentProjectilePos.y, CurrentProjectilePos.z));

		}

		return true;

	}

	float get_bullet_drop(float distance, float projectile_speed, float projectile_gravity)
	{
		if (distance < 0.001f)
			return -1;

		float m_gravity = 9.81f * projectile_gravity;
		float m_time = distance / std::fabs(projectile_speed);

		return static_cast<float>(0.5f * m_gravity * m_time * m_time);
	}

	Vector prediction(Vector from, const Vector& to, const Vector& plr_velocity, c_bullet_info inf)
	{

		Vector aimpoint = to;
		float distance = from.distance(aimpoint);

		if (distance < 0.1f) {
			return to;
		}

		float m_flBulletSpeed = inf.velocity * (inf.velocity_scale * inf.velocity_scalar);
		float bullet_gravity = inf.gravity;
		float bullet_time = distance / m_flBulletSpeed;
		float m_fDrag = inf.drag;
		float m_flBulletDrop = get_bullet_drop(distance, m_flBulletSpeed, bullet_gravity);

		const float m_flTimeStep = 0.03125f;
		float m_flYTravelled{}, m_flYSpeed{}, m_flBulletTime{}, m_flDivider{};

		float m_flDistanceTo = from.distance(aimpoint);

		Vector b_velocity{};
		Vector position = from;
		Vector gravity = Vector{ 0.f, -9.81f, 0.f };

		float partial_time = 0.f;

		Vector previous_position;
		Vector previous_velocity;

		points.clear();

		for (float distance_to_travel = 0.f; distance_to_travel < m_flDistanceTo;)
		{

			float speed_modifier = 1.f - m_flTimeStep * m_fDrag;
			m_flBulletSpeed *= speed_modifier;

			if (m_flBulletSpeed <= 0.f || m_flBulletSpeed >= 10000.f || m_flYTravelled >= 10000.f || m_flYTravelled < 0.f)
				break;

			if (m_flBulletTime > 8.f)
				break;

			m_flYSpeed += (9.81f * bullet_gravity) * m_flTimeStep;
			m_flYSpeed *= speed_modifier;

			distance_to_travel += m_flBulletSpeed * m_flTimeStep;
			m_flYTravelled += m_flYSpeed * m_flTimeStep;
			m_flBulletTime += m_flTimeStep;

		}

		Vector velocity = plr_velocity * 0.75f;
		if (velocity.y > 0.f)
			velocity.y /= 3.25f;

		std::cout << "o btime: " << bullet_time << " c btime: " << m_flBulletTime << "\n";

		aimpoint.y += m_flYTravelled;
		aimpoint += velocity * m_flBulletTime;

		return aimpoint;

	}


	Vector predicted_position(Vector target_position, Vector target_velocity, Vector local_position, c_bullet_info bullet_info)
	{

		if (!bullet_info.velocity || !bullet_info.velocity_scalar || !bullet_info.velocity_scale)
			return target_position;

		float bullet_speed = bullet_info.velocity * (bullet_info.velocity_scale * bullet_info.velocity_scalar);

		float speedmult = 1.0f - 0.015625f * bullet_info.drag;
		bullet_speed *= speedmult;

		float distance = local_position.distance(target_position);
		float travel_time = distance / bullet_speed;

		float gravity = -9.81f * bullet_info.gravity;

		Vector predicted_position;

		Vector real_velocity = target_velocity * 0.66f;

		predicted_position.x = target_position.x + (real_velocity.x) * travel_time;
		predicted_position.z = target_position.z + (real_velocity.z) * travel_time;
		predicted_position.y = target_position.y + (real_velocity.y) * travel_time - gravity * travel_time * travel_time * 0.5f;

		return predicted_position;

	}

}; inline c_ballistics ballistics;

class c_aimbot
{
private:

	float calculate_fov(Vector position)
	{

		Vector2D screen_position;

		if (!world_to_screen(position, screen_position))
			return 1000.f;

		ImVec2 display_size = ImGui::GetIO().DisplaySize;
		Vector2D screen_center(display_size.x / 2, display_size.y / 2);

		return screen_position.distance(screen_center);

	}

	Vector2D calculate_angle(Vector src, Vector dst)
	{

		#define M_PI 3.14159265358979323846f
		#define M_PI_F ((float)(M_PI))
		#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))

		Vector dir = src - dst;
		return Vector2D{ RAD2DEG(asin(dir.y / dir.length())), RAD2DEG(-atan2(dir.x, -dir.z)) };

	}

	Vector2D smooth_angle(Vector2D angles, Vector2D localViewAngles)
	{
		Vector2D delta = Vector2D(angles.x - localViewAngles.x, angles.y - localViewAngles.y);
		delta.normalize();
		float smoothing_value = 5.5f * (config.aimbot_smoothing) / 100.f;
		float smooth = powf(0.81f + smoothing_value, 0.4f);

		smooth = min(0.98f, smooth);

		Vector2D toChange = Vector2D();

		float coeff = (1.0f - smooth) / delta.Length() * 4.f;

		// fast end
		coeff = powf(coeff, 2.f) * 80.f / config.aimbot_smoothing;

		coeff = min(1.f, coeff);
		toChange = delta * coeff;

		return Vector2D(localViewAngles.x, localViewAngles.y) + toChange;
	}

	float rnd_flt(float min, float max)
	{
		static std::default_random_engine e;
		static std::uniform_real_distribution<> dis(min, max);
		return dis(e);
	}

	void do_smooth(Vector2D& end_angles, const Vector2D& prev_angles)
	{

		static auto cur_aim_time = 0.f;
		const float smoothing_amount = config.aimbot_smoothing;
		auto delta = end_angles - prev_angles;
		delta.normalize();

		if (delta.Length() == 0.f)
			return;

		const float random = rnd_flt(0.f, .1f);
		const auto final_time = delta.Length() / ((3.f + random) / 10.f);
		cur_aim_time += ImGui::GetIO().Framerate;

		if (cur_aim_time > 0.12f)
			cur_aim_time = 0.11f;

		if (cur_aim_time > final_time)
			cur_aim_time = final_time;

		delta *= cur_aim_time / final_time;

		const float xrand = rnd_flt(0.f, 0.6f * smoothing_amount);
		const float yrand = rnd_flt(0.f, 0.4f * smoothing_amount);
		delta.x += delta.x / (5.f + xrand);
		delta.y += delta.y / (2.f + yrand);

		end_angles = prev_angles + delta;
		end_angles.normalize();

	}

	void predict_position(Vector& origin, Vector velocity, float distance, c_bullet_info info)
	{

		float bullet_speed = info.velocity;

		//float speedmult = 1.0f - 0.015625f * info.drag;
		//bullet_speed *= speedmult;

		auto time = (distance / bullet_speed);

		origin.x += velocity.x * time;
		origin.y += velocity.y * time;
		origin.z += velocity.z * time;

		auto drop = 0.5f * info.gravity * time * time;
		drop += info.drag;

		std::cout << "drag: " << info.drag << " velocity: " << info.velocity << " velocity scalar: " << info.velocity_scalar << " velocity scale: " << info.velocity_scale << " gravity: " << info.gravity << "\n";

		origin.y += drop;

	}

	Vector calc_angle(const Vector& src, const Vector& dst) {
		Vector angles;

		Vector delta = src - dst;
		float hyp = delta.Length2D();

		angles.y = std::atanf(delta.y / delta.x) * 57.295779513082f;
		angles.x = std::atanf(-delta.z / hyp) * -57.295779513082f;
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return angles;
	}

	std::vector<c_aim_target> aimbot_targets{};

public:

	c_aim_target find_target()
	{

		Vector pos = memory.read<Vector>(camera.get_object() + 0x42C);

		float max_fov = config.aimbot_fov;
		int aimbot_bone = config.aimbot_bone_target; // 0 - head, 1 - body, 2 - nearest

		for (auto& entity : entitylist.entities)
		{

			if (entity.tag != unity_enums::object_tags::players)
				continue; // skip all non player entities

			entity.UpdatePosition();

			Vector2D transform_screen;

			if (!world_to_screen(entity.Position, transform_screen))
				continue; // if they're not in our immediate FOV

			game_classes::player* player = (game_classes::player*)entity.Base;

			if (!player)
				continue;

			game_classes::playermodel* model = player->get_model();

			if (player->get_lifestate() != unity_enums::life_state::Alive)
				continue; // skip dead players

			bool teammate = player->is_same_team(entitylist.local_player);

			if (teammate)
				continue;

			if (model->is_local_player())
				continue;

			switch (aimbot_bone)
			{
			case 0:
			{
				// head

				Vector head_position = bone.get_entity_bone((uintptr_t)player, (uint32_t)unity_enums::bone_list::head);

				float target_fov = calculate_fov(head_position);

				if (target_fov > max_fov)
				{
					continue; // they're out of fov
				}

				c_aim_target target;
				target.aim_point = head_position;
				target.player = player;
				target.validated = true;
				target.fov = target_fov;
				target.distance = pos.distance(head_position);

				aimbot_targets.emplace_back(target);

				break;
			}
			case 1:
			{
				// body

				Vector body_position = bone.get_entity_bone((uintptr_t)player, (uint32_t)unity_enums::bone_list::pelvis);

				float target_fov = calculate_fov(body_position);

				if (target_fov > max_fov)
					continue; // they're out of fov

				c_aim_target target;
				target.aim_point = body_position;
				target.player = player;
				target.validated = true;
				target.fov = target_fov;
				target.distance = pos.distance(body_position);

				aimbot_targets.emplace_back(target);

				break;
			}
			case 2:
			{
				// nearest

				Vector final_point;
				float final_fov;

				Vector spine_position = bone.get_entity_bone((uintptr_t)player, (uint32_t)unity_enums::bone_list::spine1);
				Vector body_position = bone.get_entity_bone((uintptr_t)player, (uint32_t)unity_enums::bone_list::pelvis);
				Vector head_position = bone.get_entity_bone((uintptr_t)player, (uint32_t)unity_enums::bone_list::head);

				float spin_fov = calculate_fov(spine_position);
				float body_fov = calculate_fov(body_position);
				float head_fov = calculate_fov(head_position);

				if (spin_fov > max_fov)
					continue; 
				if (body_fov > max_fov)
					continue; // they're out of fov
				if (head_fov > max_fov)
					continue;

				if (head_fov < body_fov || head_fov < spin_fov) {
					final_point = head_position;
					final_fov = head_fov;
				}
				else if (body_fov < head_fov || body_fov < spin_fov) {
					final_point = body_position;
					final_fov = body_fov;
				}
				else if (spin_fov < head_fov || spin_fov < body_fov) {
					final_point = spine_position;
					final_fov = spin_fov;
				}

				c_aim_target target;
				target.aim_point = final_point;
				target.player = player;
				target.validated = true;
				target.fov = final_fov;
				target.distance = pos.distance(head_position);

				aimbot_targets.emplace_back(target);

				break;
			}
			}

		}

		auto sort_func = [](c_aim_target t1, c_aim_target t2) {

			return t1.fov < t2.fov;

		};

		if (aimbot_targets.size() > 0)
			std::sort(aimbot_targets.begin(), aimbot_targets.end(), sort_func);
		else
			return c_aim_target{};

		c_aim_target best_target = aimbot_targets[0];
		aimbot_targets.clear();
		return best_target;

	}

	void run()
	{

		if (!config.enable_aimbot || !config.aimbot_bind.enabled)
			return;

		game_classes::item* itm = entitylist.local_player->get_held_item();

		if (!itm->is_weapon())
			return;

		c_aim_target target = find_target();

		if (!target.validated)
			return; // no targets were found

		if (!target.aim_point.IsValid())
			return;

		Vector pos = memory.read<Vector>(camera.get_object() + 0x42C);

		if (config.aimbot_prediction)
			if (config.prediction_type == 0)
				target.aim_point = ballistics.predicted_position(target.aim_point, target.player->get_model()->get_velocity(), pos, itm->projectile()->get_bullet_info());
			else
				target.aim_point = ballistics.prediction(pos, target.aim_point, target.player->get_model()->get_velocity(), itm->projectile()->get_bullet_info());

		Vector2D aim_angle = calculate_angle(pos, target.aim_point);

		if (config.aimbot_smoothing_enable)
			aim_angle = smooth_angle(aim_angle, entitylist.local_player->get_input()->get_viewangles());

		aim_angle.normalize();

		if (config.aimbot_silent)
			entitylist.local_player->get_eyes()->set_body_rotation(aim_angle);
		else if (!config.aimbot_silent)
			entitylist.local_player->get_input()->set_viewangles(aim_angle);

	}

}; inline c_aimbot aimbot;