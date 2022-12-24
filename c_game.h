#pragma once

#include <cstdint>
#include <mutex>

#include "types/vector4.h"

#include "c_memory.h"
#include "types/vector3.h"
#include "types/matrix.h"
#include "types/unity_types.h"

#include <xmmintrin.h>
#include <intrin.h>
#include <string>
#include <algorithm>

namespace n_offsets
{

	// "Signature": "System_Collections_Generic_List_BaseGameMode__c*"
	constexpr uintptr_t basegamemode_list = 56833128;

	// MainCamera_c
	constexpr uintptr_t camera_c = 56844744;


}

struct vmatrix_t
{
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};
};

struct ProjectileWeaponModModifier
{
	bool enabled;
	float scalar;
	float offset;
};

struct c_bullet_info
{
	float velocity = 333;
	float drag = 1.f;
	float gravity = 1.f;
	float velocity_scale = 1.f;
	float velocity_scalar = 1.f;
};

class camera_t
{
private:
	Vector position{};
	vmatrix_t vmatrix{};
public:
	bool set_position(Vector _position)
	{
		position = _position;
		return (bool)(position.x && position.y && position.z);
	}

	void set_vmatrix(vmatrix_t _vmatrix)
	{
		vmatrix = _vmatrix;
	}

	Vector get_position()
	{
		return position;
	}

	vmatrix_t get_vmatrix()
	{
		return vmatrix;
	}

	uintptr_t get_object()
	{

		uintptr_t cam = memory.chain<uintptr_t>(memory.game_assembly, { n_offsets::camera_c, 0xB8, 0x0, 0x10 });
		return cam;

	}
} camera;

#include "types/vector2.h"

bool world_to_screen(const Vector& entity_position, Vector2D& screen_position)
{

	Vector transform{ camera.get_vmatrix()._14, camera.get_vmatrix()._24, camera.get_vmatrix()._34 };
	Vector right{ camera.get_vmatrix()._11, camera.get_vmatrix()._21, camera.get_vmatrix()._31 };
	Vector up{ camera.get_vmatrix()._12, camera.get_vmatrix()._22, camera.get_vmatrix()._32 };

	float w = transform.Dot(entity_position) + camera.get_vmatrix()._44;

	Vector2D pos{ right.Dot(entity_position) + camera.get_vmatrix()._41, up.Dot(entity_position) + camera.get_vmatrix()._42 };

	if (w < 0.098f)
		return false;

	static float screen_center_x = GetSystemMetrics(SM_CXSCREEN) / 2;
	static float screen_center_y = GetSystemMetrics(SM_CYSCREEN) / 2;

	screen_position = Vector2D(screen_center_x * (1 + pos.x / w), screen_center_y * (1 - pos.y / w));

	return true;

}

#define cast_this (uintptr_t)this

uintptr_t get_component(uintptr_t game_object, const char* name_str)
{
	if (!game_object)
		return NULL;

	uintptr_t list = memory.read<uintptr_t>(game_object + 0x30);
	for (int i = 0; i < 20; i++)
	{
		uintptr_t component = memory.read<uintptr_t>(list + (0x10 * i + 0x8));

		if (!component)
			continue;

		uintptr_t unk1 = memory.read<uintptr_t>(component + 0x28);

		if (!unk1)
			continue;

		uintptr_t name_ptr = memory.read<uintptr_t>(unk1);
		std::string name = memory.read_str(memory.read<uintptr_t>(name_ptr + 0x10), 18);

		if (!strcmp(name.c_str(), name_str))
			return unk1;
	}

	return NULL;
}

namespace game_classes
{

	float clamp(float value)
	{
		float result;
		if (value < 0.f)
		{
			result = 0.f;
		}
		else if (value > 1.f)
		{
			result = 1.f;
		}
		else
		{
			result = value;
		}
		return result;
	}

	float lerp(Vector in)
	{
		return in.x + (in.y - in.x) * clamp(in.z);
	}

	double to_rad(double degree)
	{
		double pi = 3.14159265359;
		return (degree * (pi / 180));
	}

	Vector4D to_quat(Vector Euler)
	{
		double heading = to_rad(Euler.x);
		double attitude = to_rad(Euler.y);
		double bank = to_rad(Euler.z);

		double c1 = cos(heading / 2);
		double s1 = sin(heading / 2);
		double c2 = cos(attitude / 2);
		double s2 = sin(attitude / 2);
		double c3 = cos(bank / 2);
		double s3 = sin(bank / 2);
		double c1c2 = c1 * c2;
		double s1s2 = s1 * s2;
		Vector4D quat;
		quat.w = c1c2 * c3 - s1s2 * s3;
		quat.x = c1c2 * s3 + s1s2 * c3;
		quat.y = s1 * c2 * c3 + c1 * s2 * s3;
		quat.z = c1 * s2 * c3 - s1 * c2 * s3;
		return { quat.y, quat.z, (quat.x * -1), quat.w };
	}

	class item_definition
	{
	public:

		int stack()
		{
			return memory.read<int>((uintptr_t)this + 0x58);
		}

		std::string get_real_name()
		{
			uintptr_t nameptr = memory.read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x20);
			std::wstring shortname = memory.read_wstr(nameptr + 0x14);
			return std::string(shortname.begin(), shortname.end());
		}
		std::string get_name()
		{
			uintptr_t translation = memory.read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x28);
			uintptr_t nameptr = memory.read<uintptr_t>(translation + 0x18);
			std::wstring shortname = memory.read_wstr(nameptr + 0x14);

			// shotgun filter
			{
				std::wstring shotgun_filter = L" Shotgun";
				size_t shotgun_pos = shortname.find(shotgun_filter);
				if (shotgun_pos != std::string::npos)
					shortname.erase(shotgun_pos, shotgun_filter.length());
			}
			// shorten some names more
			{
				std::wstring automatic_filter = L"Automatic "; // so we can keep 'semi-rifle', 'semi-pistol', etc...
				size_t automatic_pos = shortname.find(automatic_filter);
				if (automatic_pos != std::string::npos)
					shortname.erase(automatic_pos, automatic_filter.length());

				std::wstring lr_300_filter = L"LR-300 Assault Rifle"; // shorten to 'LR-300'
				size_t lr_300_pos = shortname.find(lr_300_filter);
				if (lr_300_pos != std::string::npos)
					shortname = L"LR-300";
			}

			//std::cout << std::string(shortname.begin(), shortname.end());

			return (shortname.length() > 128 || shortname.empty()) ? "None" : std::string(shortname.begin(), shortname.end());
		}
	};

	class magazine
	{
	public:
		int get_capacity()
		{
			return memory.read<int>((uintptr_t)this + 0x18);
		}
		int get_contents()
		{
			return memory.read<int>((uintptr_t)this + 0x1C);
		}
		item_definition* get_definition()
		{
			return memory.read<item_definition*>((uintptr_t)this + 0x20);
		}
	};

	class recoil_properties
	{
	public:
		recoil_properties* get_override()
		{

			if (!this)
				return nullptr;

			return memory.read<recoil_properties*>((uintptr_t)this + 0x78);
		}
		void set_aimcone_scale(float set)
		{
			if (!this)
				return ;
			memory.write<float>((uintptr_t)this + 0x60, set);
		}
		void null_recoil()
		{
			if (!this)
				return ;
			memory.write<Vector4D>((uintptr_t)this + 0x18, { 0.f, 0.f, 0.f, 0.f });
		}
	};

	class item_mod_projectile
	{
	public:
		void set_spread(float set)
		{
			memory.write<float>(cast_this + 0x30, set);
			memory.write<float>(cast_this + 0x38, set);
		}
	};

	class projectile
	{
	public:
		Vector get_current_velocity()
		{
			return memory.read<Vector>((uintptr_t)this + 0x118);
		}
		Vector get_current_position()
		{
			return memory.read<Vector>((uintptr_t)this + 0x124);
		}
		float get_travelled_time()
		{
			return memory.read<float>((uintptr_t)this + 0x134);
		}
		void set_thickness(float set)
		{
			memory.write<float>((uintptr_t)this + 0x2C, set);
		}
		item_mod_projectile* get_mod()
		{
			return memory.read<item_mod_projectile*>(cast_this + 0xE8);
		}
	};

	class base_projectile
	{
	public:
		recoil_properties* get_recoil()
		{

			if (!this)
				return nullptr;

			return memory.read<recoil_properties*>((uintptr_t)this + 0x2E0);
		}
		void set_velocity_scale(float set)
		{
			if (!this)
				return;
			memory.write<float>((uintptr_t)this + 0x28C, set);
		}
		magazine* get_magazine()
		{

			if (!this)
				return nullptr;

			return memory.read<magazine*>((uintptr_t)this + 0x2C0);
		}
		void null_spread()
		{
			if (!this)
				return;

			memory.write<Vector4D>((uintptr_t)this + 0x2F0, {-3.f, -3.f, -3.f, -3.f});
		}
		void set_automatic(bool set)
		{

			if (!this)
				return;

			memory.write<bool>((uintptr_t)this + 0x290, set);
		}

		c_bullet_info get_bullet_info()
		{
			c_bullet_info info{};

			info.velocity_scale = memory.read<float>(reinterpret_cast<uintptr_t>(this) + 0x28C);

			uintptr_t magazine = memory.read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x2C0);
			uintptr_t unity_class = memory.chain<uintptr_t>(magazine, { 0x20, 0x10, 0x30 });
			uintptr_t itemModProjectile = get_component(unity_class, "ItemModProjectile");

			info.velocity = memory.read<float>(itemModProjectile + 0x34);

			uintptr_t projectileObject = memory.read<uintptr_t>(itemModProjectile + 0x18);

			uintptr_t unk0 = memory.read<uintptr_t>(projectileObject + 0x18);
			uintptr_t unk1 = memory.read<uintptr_t>(unk0 + 0x10);

			uintptr_t projectile = get_component(unk1, "Projectile");

			Vector2D projectile_info = memory.read<Vector2D>(projectile + 0x24);

			info.drag = projectile_info.x;
			info.gravity = projectile_info.y;

			uintptr_t children_list = memory.read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x40);

			int children_list_size = memory.read<int>(children_list + 0x18);
			children_list = memory.read<uintptr_t>(children_list + 0x10);

			for (int i = 0; i < children_list_size; ++i)
			{
				uintptr_t child_entity = memory.read<uintptr_t>(children_list + (0x20 + (i * 0x8)));
				std::string child_entity_name = memory.read_str(memory.read<uintptr_t>(child_entity + 0x10));

				if (child_entity_name == "ProjectileWeaponMod")
				{
					ProjectileWeaponModModifier velocity_scalar = memory.read<ProjectileWeaponModModifier>((child_entity + 0x178));

					if (velocity_scalar.enabled)
						info.velocity_scalar = velocity_scalar.scalar;
				}
			}

			return info;
		}

		projectile* get_projectile()
		{

			uintptr_t magazine = memory.read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x2C0);
			uintptr_t unity_class = memory.chain<uintptr_t>(magazine, { 0x20, 0x10, 0x30 });
			uintptr_t itemModProjectile = get_component(unity_class, "ItemModProjectile");

			uintptr_t projectileObject = memory.read<uintptr_t>(itemModProjectile + 0x18);

			uintptr_t unk0 = memory.read<uintptr_t>(projectileObject + 0x18);
			uintptr_t unk1 = memory.read<uintptr_t>(unk0 + 0x10);

			uintptr_t _projectile = get_component(unk1, "Projectile");

			return (projectile*)_projectile;

		}

		uintptr_t get_created_projectiles()
		{

			return memory.read<uintptr_t>((uintptr_t)this + 0x370);

		}

	};

	class item
	{
	public:
		int get_id()
		{
			return memory.read<int>((uintptr_t)this + 0x28);
		}
		base_projectile* projectile()
		{

			if (!this)
				return nullptr;

			return memory.read<base_projectile*>((uintptr_t)this + 0xA0);
		}
		bool is_weapon()
		{
			auto ItemName = get_definition()->get_real_name();

			if (ItemName.find("rifle") != std::string::npos) return true;
			if (ItemName.find("pistol") != std::string::npos) return true;
			if (ItemName.find("bow") != std::string::npos) return true;
			if (ItemName.find("lmg") != std::string::npos) return true;
			if (ItemName.find("shotgun") != std::string::npos) return true;
			if (ItemName.find("smg") != std::string::npos) return true;
			else return false;
		}
		item_definition* get_definition()
		{

			if (!this)
				return nullptr;

			return memory.read<item_definition*>((uintptr_t)this + 0x20);
		}
		int get_amount()
		{
			return memory.read<int>((uintptr_t)this + 0x30);
		}
	};

	class entity
	{
	public:

		entity() {};

		entity(uintptr_t _base, uintptr_t _transform, uintptr_t _gameobject, unity_enums::object_tags _tag)
		{

			this->Base = _base;
			this->Transform = _transform;
			this->GameObject = _gameobject;
			this->tag = _tag;

			if (tag != unity_enums::object_tags::players)
			{

				this->name = memory.read_str(memory.read<uintptr_t>(_gameobject + 0x60), 1024);

				/* for dropped items */
				if (this->name.find("item_drop.prefab") != std::string::npos)
					this->name = "item drop";

				/* removes (world)*/
				std::string world_str = " (world)";
				if (this->name.find(world_str) != std::string::npos)
					this->name.erase(this->name.find(world_str), world_str.length());

				/* changes dots to spaces between names */
				std::replace(this->name.begin(), this->name.end(), '.', ' '); // replace all dots in name with a space, looks nicer

			}

			UpdatePosition();

		}

		void UpdatePosition()
		{

			if (this->Transform)
				this->Position = memory.read<Vector>(this->Transform + 0x90);

		}

		uintptr_t Base{};
		uintptr_t Transform{};
		uintptr_t GameObject{};
		Vector Position;
		UINT prefabID{};
		std::string name = "invalid";

		unity_enums::object_tags tag;

	};

	class gameobject
	{
	public:
		Vector get_position()
		{

			uintptr_t comp = memory.read<uintptr_t>((uintptr_t)this + 0x30);
			uintptr_t comp2 = memory.read<uintptr_t>((uintptr_t)this + 0x8);
			uintptr_t transform = memory.read<uintptr_t>((uintptr_t)this + 0x38);
			return memory.read<Vector>(transform + 0x90);

		}
	};

	class eyes
	{
	public:
		void set_viewoffset(Vector goop)
		{
			memory.write<Vector>((uintptr_t)this + 0x38, goop);
		}
		void set_body_rotation(Vector2D set)
		{
			memory.write<Vector4D>((uintptr_t)this + 0x44, to_quat({set.x, set.y, 0.f}));
		}
		Vector4D get_body_rotation()
		{
			return memory.read<Vector4D>((uintptr_t)this + 0x44);
		}
	};

	class inputmessage
	{
	public:
		void set_aim_angle(Vector set)
		{
			memory.write<Vector>((uintptr_t)this + 0x18, set);
		}
		Vector get_aim_angle()
		{
			return memory.read<Vector>((uintptr_t)this + 0x18);
		}
	};

	class inputstate
	{
	public:
		inputmessage* get_current()
		{
			return memory.read<inputmessage*>((uintptr_t)this + 0x10);
		}
	};

	class playerinput
	{
	public:
		void set_viewangles(Vector2D goop)
		{
			memory.write<Vector2D>((uintptr_t)this + 0x3C, goop);
		}
		Vector2D get_viewangles()
		{
			return memory.read<Vector2D>((uintptr_t)this + 0x3C);
		}
		Vector get_bodyangles()
		{
			return memory.read<Vector>((uintptr_t)this + 0x3C);
		}
		inputstate* get_inputstate()
		{
			return memory.read<inputstate*>((uintptr_t)this + 0x20);
		}
		void set_body_angles(Vector set)
		{
			memory.write<Vector>(cast_this + 0x58, set);
		}
		void set_head_rotation(Vector2D set)
		{
			memory.write<Vector4D>(cast_this + 0x2C, to_quat({ set.x, set.y, 0.f }));
		}
		void set_body_rotation(Vector2D set)
		{
			//0x2C
			memory.write<Vector4D>((uintptr_t)this + 0x2C, to_quat({ set.x, set.y, 0.f }));
		}
	};

	class player_nametag
	{
	public:
		Vector position_offset()
		{
			return memory.read<Vector>((uintptr_t)this + 0x38);
		}
	};

	class modelstate
	{
	public:
		int flags()
		{
			return memory.read<int>((uintptr_t)this + 0x24);
		}
		void set_flag(int set)
		{
			memory.write<int>((uintptr_t)this + 0x24, set);
		}
		bool is_sprinting()
		{
			return flags() & (int)unity_enums::model_flags::Sprinting;
		}
		bool is_ducked()
		{
			return flags() & (int)unity_enums::model_flags::Ducked;
		}
		bool is_swimming()
		{
			return memory.read<float>(reinterpret_cast<uintptr_t>(this) + 0x14) >= 0.65f;
		}
	};

	class playermodel
	{
	public:
		bool is_npc()
		{
			return memory.read<bool>((uintptr_t)this + 0x320);
		}
		bool has_held_entity()
		{
			return memory.read<bool>((uintptr_t)this + 0x2F4);
		}
		bool is_local_player()
		{
			return memory.read<bool>((uintptr_t)this + 0x299);
		}
		modelstate* get_modelstate()
		{
			return memory.read<modelstate*>((uintptr_t)this + 0x210);
		}
		player_nametag* get_nametag()
		{
			return memory.read<player_nametag*>((uintptr_t)this + 0x290);
		}
		bool visible()
		{
			return memory.read<bool>((uintptr_t)this + 0x288);
		}
		void set_acceleration(int set)
		{
			memory.write<int>((uintptr_t)this + 0x3C, set);
		}
		box_collider get_collider()
		{
			return memory.read<box_collider>((uintptr_t)this + 0x78);
		}
		Vector get_position()
		{
			return memory.read<Vector>((uintptr_t)this + 0x218);
		}
		gameobject* get_head_bone()
		{
			return memory.read<gameobject*>((uintptr_t)this + 0xA0);
		}
		Vector get_velocity()
		{
			return memory.read<Vector>((uintptr_t)this + 0x23C);
		}
	};

	class basemovement
	{
	public:
		void set_running(float set)
		{
			memory.write<float>((uintptr_t)this + 0x40, set);
		}
		void set_next_sprint(float set)
		{
			memory.write<float>((uintptr_t)this + 0x12C, set);
		}
		void set_last_sprint(float set)
		{
			memory.write<float>((uintptr_t)this + 0x130, set);
		}
		Vector get_target_movement()
		{
			return memory.read<Vector>((uintptr_t)this + 0x34);
		}
		void set_target_movement(Vector set)
		{
			memory.write<Vector>((uintptr_t)this + 0x34, set);
		}
		void set_grounded(float set)
		{
			memory.write<float>((uintptr_t)this + 0x4C, set);
		}
		void setGroundAngle(float angle)
		{
			memory.write<float>(reinterpret_cast<uintptr_t>(this) + 0xC4, angle);
		}
		void setGroundAngleNew(float angle)
		{
			memory.write<float>(reinterpret_cast<uintptr_t>(this) + 0xC8, angle);
		}
		void set_flying(bool set)
		{
			memory.write<bool>((uintptr_t)this + 0x149, set);
		}
		bool get_flying()
		{
			return memory.read<bool>((uintptr_t)this + 0x149);
		}
		void setJumpTime(float jump_time)
		{
			memory.write<float>(reinterpret_cast<uintptr_t>(this) + 0xD0, jump_time);
		}

		void setMaxAngleClimb(float set)
		{
			memory.write<float>(reinterpret_cast<uintptr_t>(this) + 0x90, set);
		}

		void setMaxAngleWalk(float set)
		{
			memory.write<float>(reinterpret_cast<uintptr_t>(this) + 0x8C, set);
		}

		void setGroundTime(float ground_time)
		{
			memory.write<float>(reinterpret_cast<uintptr_t>(this) + 0xCC, ground_time);
		}

		void setLandTime(float land_time)
		{
			memory.write<float>(reinterpret_cast<uintptr_t>(this) + 0xD4, land_time);
		}
	};

	struct transform_access_read_only_t
	{
		uint64_t transform_data{};
	};

	struct transform_data_t
	{
		uint64_t transform_array{};
		uint64_t transform_indices{};
	};

	struct matrix34_t
	{
		Vector4D vec0{};
		Vector4D vec1{};
		Vector4D vec2{};
	};

	Vector __vectorcall get_bone_position(uintptr_t pTransform)
	{
		__m128 result{};

		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		transform_access_read_only_t transform_access_read_only = memory.read<transform_access_read_only_t>(pTransform + 0x38);
		unsigned int index = memory.read<unsigned int>(pTransform + 0x40);
		transform_data_t transform_data = memory.read<transform_data_t>(transform_access_read_only.transform_data + 0x18);

		if (transform_data.transform_array && transform_data.transform_indices)
		{
			result = memory.read<__m128>(transform_data.transform_array + 0x30 * index);
			int transform_index = memory.read<int>(transform_data.transform_indices + 0x4 * index);
			int safe = 0;
			while (transform_index >= 0 && safe++ < 200)
			{
				matrix34_t matrix = memory.read<matrix34_t>(transform_data.transform_array + 0x30 * transform_index);

				__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x00));	// xxxx
				__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x55));	// yyyy
				__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x8E));	// zwxy
				__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0xDB));	// wzyw
				__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0xAA));	// zzzz
				__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x71));	// yxwy
				__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix.vec2), result);

				result = _mm_add_ps(_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), *(__m128*)(&matrix.vec0));

				transform_index = memory.read<int>(transform_data.transform_indices + 0x4 * transform_index);
			}
		}

		return Vector(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}

	class mounted
	{
	public:
		void can_hold_items(bool set)
		{
			memory.write<bool>((uintptr_t)this + 0x2B8, set);
		}
		bool get_can_hold_items()
		{
			return memory.read<bool>(cast_this + 0x2B8);
		}
	};

	class player_tick
	{
	public:
		Vector get_position()
		{
			return memory.read<Vector>(cast_this + 0x20);
		}
	};

	class player
	{

	public:

		uint32_t get_team()
		{
			return memory.read<uint32_t>((uintptr_t)this + 0x5A0);
		}

		bool is_same_team(player* other)
		{
			uint32_t our_team = get_team();
			uint32_t their_team = other->get_team();

			if (!our_team || !their_team)
				return false;

			return our_team == their_team;

		}

		player_tick* get_last_tick()
		{
			return memory.read<player_tick*>((uintptr_t)this + 0x668);
		}

		bool is_visible()
		{
			return memory.read<bool>((uintptr_t)this + 0xD8);
		}

		float get_speed(float running, float ducking)
		{

			float num = 1.f;
			num -= memory.read<float>(reinterpret_cast<uintptr_t>(this) + 0x764);

			modelstate* modelstate = get_modelstate();

			if (modelstate->is_swimming())
			{
				num += memory.read<float>(reinterpret_cast<uintptr_t>(this) + 0x768);
			}

			return lerp({ lerp({ 2.8f, 5.5f, running }), 1.7f, ducking }) * num;

		}

		float last_time_seen()
		{
			return memory.read<float>(cast_this + 0x674);
		}

		mounted* get_mounted()
		{
			return memory.read<mounted*>(cast_this + 0x608);
		}

		void omni_sprint()
		{

			auto model_state = get_modelstate();
			basemovement* movement = get_movement();

			int m_bBusy = model_state->is_ducked() | model_state->is_swimming();

			float m_flSpeed = get_speed(1.f, m_bBusy);

			Vector m_vecVelocity = movement->get_target_movement();
			if (m_vecVelocity.IsValid())
			{
				Vector m_vecXZ = Vector{ m_vecVelocity.x, 0.f, m_vecVelocity.z }.Normalized() * m_flSpeed;
				m_vecVelocity = { m_vecXZ.x, m_vecVelocity.y, m_vecXZ.z };

				//model_state->set_flag((int)unity_enums::model_flags::Sprinting);
				movement->set_running(1.f);
				model_state->set_flag(16);
				movement->set_target_movement(m_vecVelocity * 0.97f);
				model_state->set_flag(16);
				movement->set_running(1.f);
				//model_state->set_flag((int)unity_enums::model_flags::Sprinting);
			}

		}

		eyes* get_eyes()
		{
			return memory.read<eyes*>((uintptr_t)this + 0x698);
		}

		playerinput* get_input()
		{
			return memory.read<playerinput*>((uintptr_t)this + 0x4F0);
		}

		playermodel* get_model()
		{
			return memory.read<playermodel*>((uintptr_t)this + 0x4D0);
		}

		modelstate* get_modelstate()
		{
			return memory.read<modelstate*>((uintptr_t)this + 0x600);
		}

		int get_flags()
		{

			return memory.read<int>((uintptr_t)this + 0x690);

		}

		void set_flag(int set)
		{
			int curflags = get_flags();
			memory.write<int>((uintptr_t)this + 0x690, curflags |= set);
		}

		void remove_flag(int set)
		{
			int curflags = get_flags();
			memory.write<int>((uintptr_t)this + 0x690, curflags &= ~(set));
		}

		int get_active_id()
		{
			return memory.read<int>((uintptr_t)this + 0x5D8);
		}

		item* get_held_item()
		{

			int active_id = get_active_id();

			uintptr_t inventory = memory.read<uintptr_t>((uintptr_t)this + 0x6A0);

			uintptr_t container_belt = memory.read<uintptr_t>(inventory + 0x28);

			uintptr_t contents = memory.read<uintptr_t>(container_belt + 0x38);

			int size = memory.read<int>(contents + 0x18);

			contents = memory.read<uintptr_t>(contents + 0x10);

			for (int i = 0; i < size; i++)
			{

				item* _item = memory.read<item*>(contents + (0x20 + (i * 0x8)));

				if (!_item)
					continue;

				if (_item->get_id() == active_id)
					return _item;

			}

			return NULL;

		}

		uintptr_t get_belt()
		{

			uintptr_t inventory = memory.read<uintptr_t>((uintptr_t)this + 0x6A0);

			uintptr_t container_belt = memory.read<uintptr_t>(inventory + 0x28);

			uintptr_t contents = memory.read<uintptr_t>(container_belt + 0x38);

			return contents;

		}

		unity_enums::life_state get_lifestate()
		{
			return memory.read<unity_enums::life_state>((uintptr_t)this + 0x224);
		}

		Vector get_looking_at()
		{
			return memory.read<Vector>((uintptr_t)this + 0x520);
		}

		basemovement* get_movement()
		{
			return memory.read<basemovement*>((uintptr_t)this + 0x4F8);
		}

		std::string get_name()
		{

			unity_string* t2 = memory.read<unity_string*>((uintptr_t)this + 0x6F0);
			unity_string ustr = memory.read<unity_string>((uintptr_t)t2);
			std::wstring tmp(ustr.buffer);
			return std::string(tmp.begin(), tmp.end());

		}

		void set_tick_interval(float set)
		{
			memory.write<float>((uintptr_t)this + 0x658, set);
		}
		float get_tick_interval()
		{
			return memory.read<float>((uintptr_t)this + 0x658);
		}

		void set_tick_rate(float set)
		{
			memory.write<float>((uintptr_t)this + 0x650, set);
		}

		Vector get_bone(unity_enums::bone_list bone)
		{

			uintptr_t bone_transforms = memory.read<uintptr_t>((uintptr_t)get_model() + 0x48); // Model->boneTransforms
			uintptr_t entity_bone = memory.read<uintptr_t>(bone_transforms + (0x20 + ((uint32_t)bone * 0x8)));
			uintptr_t _bone = memory.read<uintptr_t>(entity_bone + 0x10);
			return get_bone_position(_bone);

		}

		float get_health()
		{
			return memory.read<float>((uintptr_t)this + 0x22C);
		}

		boundingbox get_bounds_box()
		{

			if (!this)
				return boundingbox{};

			boundingbox box{};

			playermodel* model = get_model();
			Vector tmp_pos = model->get_position();

			Vector head = (tmp_pos + Vector(0, model->get_modelstate()->flags() & (int)unity_enums::model_flags::Ducked ? 1.1f : 1.8f, 0));
			Vector feet = (tmp_pos + tmp_pos) / 2.f;
			Vector2D tempFeet, tempHead;

			if (world_to_screen(head, tempHead) && world_to_screen(feet, tempFeet))
			{

				float height = (tempHead.y - tempFeet.y);
				float width = height / 4.0f;

				float Entity_x = tempFeet.x - width;
				float Entity_y = tempFeet.y;
				float Entity_w = height / 2;

				box.valid = true;

				box.x = Entity_x;
				box.y = Entity_y;
				box.w = Entity_w;
				box.h = height;

				box.head_pos = tempHead;
				box.l_foot_pos = tempFeet;

			}

			return box;

		}

	};

}

class bone_t
{
private:
	struct transform_access_read_only_t
	{
		uint64_t transform_data{};
	};

	struct transform_data_t
	{
		uint64_t transform_array{};
		uint64_t transform_indices{};
	};

	struct matrix34_t
	{
		Vector4D vec0{};
		Vector4D vec1{};
		Vector4D vec2{};
	};

	__forceinline Vector __fastcall get_bone_position(uintptr_t pTransform)
	{
		__m128 result{};

		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		transform_access_read_only_t transform_access_read_only = memory.read<transform_access_read_only_t>(pTransform + 0x38);
		unsigned int index = memory.read<unsigned int>(pTransform + 0x40);
		transform_data_t transform_data = memory.read<transform_data_t>(transform_access_read_only.transform_data + 0x18);

		if (transform_data.transform_array && transform_data.transform_indices)
		{
			result = memory.read<__m128>(transform_data.transform_array + 0x30 * index);
			int transform_index = memory.read<int>(transform_data.transform_indices + 0x4 * index);
			int safe = 0;
			while (transform_index >= 0 && safe++ < 25)
			{
				matrix34_t matrix = memory.read<matrix34_t>(transform_data.transform_array + 0x30 * transform_index);

				__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x00));	// xxxx
				__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x55));	// yyyy
				__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x8E));	// zwxy
				__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0xDB));	// wzyw
				__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0xAA));	// zzzz
				__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix.vec1), 0x71));	// yxwy
				__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix.vec2), result);

				result = _mm_add_ps(_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
						tmp7)), *(__m128*)(&matrix.vec0));

				transform_index = memory.read<int>(transform_data.transform_indices + 0x4 * transform_index);
			}
		}

		return Vector(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}
public:
	Vector get_entity_bone(uintptr_t player, int bone_index)
	{
		uintptr_t player_model = memory.read<uintptr_t>(player + 0x130); // BaseEntity->model
		uintptr_t bone_transforms = memory.read<uintptr_t>(player_model + 0x48); // Model->boneTransforms
		uintptr_t entity_bone = memory.read<uintptr_t>(bone_transforms + (0x20 + (bone_index * 0x8)));
		uintptr_t bone = memory.read<uintptr_t>(entity_bone + 0x10);
		return get_bone_position(bone);
	}
} bone;

uintptr_t get_class(uintptr_t typeinfo)
{

	return memory.chain<uintptr_t>(memory.game_assembly, { typeinfo, 0xB8, 0x0 });

}

uint64_t scan_for_class(const char* name) {
	auto base = memory.game_assembly;
	auto dos_header = memory.read<IMAGE_DOS_HEADER>(base);
	auto data_header = memory.read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (3 * 40));
	auto next_section = memory.read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (4 * 40));
	auto data_size = next_section.VirtualAddress - data_header.VirtualAddress;

	if (strcmp((char*)data_header.Name, ".data")) {
		printf("[!] Section order changed\n");
		exit(0);
	}

	for (uint64_t offset = data_size; offset > 0; offset -= 8) {
		char klass_name[0x100] = { 0 };
		auto klass = memory.read<uint64_t>(base + data_header.VirtualAddress + offset);
		if (klass == 0) { continue; }
		auto name_pointer = memory.read<uint64_t>(klass + 0x10);
		if (name_pointer == 0) { continue; }
		memory.read(name_pointer, klass_name, sizeof(klass_name));
		if (!strcmp(klass_name, name)) {
			printf("[*] 0x%x -> %s\n", data_header.VirtualAddress + offset, name);
			return klass;
		}
	}

	printf("[!] Unable to find %s in scan\n", name);
	exit(0);
}

class c_entitylist
{
public:

	std::mutex sync{};
	std::vector<game_classes::entity> entities{};
	game_classes::player* local_player{};

	void get_local_player()
	{
		uintptr_t localplayer = get_class(56847416);
		local_player = (game_classes::player*)localplayer;
	}

	UINT_PTR GetBaseGameModeList()
	{
		UINT_PTR BaseGameModeList = 0;
		UINT_PTR BaseGameModeList_Static_Objects = 0;

		while (!BaseGameModeList)
		{
			BaseGameModeList = memory.read<UINT_PTR>(memory.game_assembly + n_offsets::basegamemode_list);
			if (!BaseGameModeList) { Sleep(5); continue; }
		}

		while (!BaseGameModeList_Static_Objects)
		{
			BaseGameModeList_Static_Objects = memory.read<UINT_PTR>(BaseGameModeList + 0xB8);
			if (!BaseGameModeList_Static_Objects) { Sleep(5); continue; }
		}

		return BaseGameModeList_Static_Objects;
	}

	UINT_PTR GetClientEntities()
	{
		UINT_PTR clientEntities = 0;

		while (!clientEntities)
		{
			clientEntities = memory.read<UINT_PTR>(GetBaseGameModeList() + 0x20);
			if (!clientEntities) { Sleep(5); continue; }
		}

		return clientEntities;
	}

	UINT_PTR GetEntityRealm()
	{
		UINT_PTR entityRealm = 0;

		while (!entityRealm)
		{
			entityRealm = memory.read<UINT_PTR>(GetClientEntities() + 0x10);
			if (!entityRealm) { 
				Sleep(5);
				local_player = nullptr;
				continue; 
			}
		}

		return entityRealm;
	}

	UINT_PTR GetBufferList()
	{
		UINT_PTR bufferList = 0;

		while (!bufferList)
		{
			bufferList = memory.read<UINT_PTR>(GetEntityRealm() + 0x28);
			if (!bufferList) { Sleep(5); continue; }
		}

		return bufferList;
	}

	UINT_PTR GetObjectList()
	{
		UINT_PTR objectList = 0;

		while (!objectList)
		{
			objectList = memory.read<UINT_PTR>(GetBufferList() + 0x18);
			if (!objectList) { Sleep(5); continue; }
		}

		return objectList;
	}

	int GetBufferListSize()
	{
		int objectListSize = 0;
		objectListSize = memory.read<int>(GetBufferList() + 0x10);
		return objectListSize;
	}

	void UpdateEntityList()
	{
		while (true)
		{

			std::vector<game_classes::entity> t_entityList;

			get_local_player();

			int entityCount = GetBufferListSize();

			for (int i = 0; i < entityCount; i++)
			{
				uintptr_t baseNetworkable = memory.read<uintptr_t>(GetObjectList() + 0x20 + ((UINT_PTR)i * 0x8));
				if (!baseNetworkable) continue;

				uintptr_t m_CachedPtr = memory.read<uintptr_t>(baseNetworkable + 0x10);// internal unity component (Unity::Component)m_CachedPtr (or look at Scripting::GetCachedPtrFromScriptingWrapper)
				if (!m_CachedPtr) continue;

				uintptr_t gameObject = memory.read<uintptr_t>(m_CachedPtr + 0x30);// gameObject
				if (!gameObject) continue;

				unity_enums::layer m_Layer = memory.read<unity_enums::layer>(gameObject + 0x50);// m_Layer
				if (m_Layer == unity_enums::layer::Tree) continue;

				unity_enums::object_tags tag = memory.read<unity_enums::object_tags>(gameObject + 0x54);

				uintptr_t m_Component = memory.read<uintptr_t>(gameObject + 0x30);// component array gameObject.dynamic_array<GameObject::ComponentPair,0> m_Component 
				if (!m_Component) continue;

				uintptr_t component = memory.read<uintptr_t>(m_Component + 0x8);// first index of the component array (ImmediatePtr<Unity::Component> component)
				if (!component) continue;

				uintptr_t transform = memory.read<uintptr_t>(component + 0x38);// transform components internal transform address
				if (!transform) continue;

				game_classes::entity entity(baseNetworkable, transform, gameObject, tag);

				entity.prefabID = memory.read<unsigned int>(baseNetworkable + 0x4C);

				t_entityList.emplace_back(entity);
			}

			sync.lock();
			entities.clear();
			entities = t_entityList;
			sync.unlock();

			Sleep(500);
		}
	}

}; inline c_entitylist entitylist;

class c_unityengine
{
public:

}; inline c_unityengine unityengine;