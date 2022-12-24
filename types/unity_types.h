#pragma once

#include "../c_memory.h"
#include "vector3.h"
#include "vector2.h"

struct unity_string
{
	char pad[0x10];
	int length;
	wchar_t buffer[128 + 1];
};

struct box_collider
{
	Vector center;
	Vector size;
};

struct boundingbox
{

	bool valid = false;
	float x = 0.f, y = 0.f, w = 0.f, h = 0.f;
	Vector2D top_left{};
	Vector2D bottom_right{};
	Vector2D head_pos{};
	Vector2D l_foot_pos{};
	Vector2D r_foot_pos{};

};

template <typename type>
class unity_list
{
public:

	type get_list(uint32_t index)
	{

		const uintptr_t internal_list = memory.read<uintptr_t>((uintptr_t)this + 0x20);
		const uintptr_t unk = memory.read<type*>(internal_list + index * sizeof(type));

		return memory.read<type>(unk);

	}

	type get_value(uint32_t index)
	{

		const uintptr_t list = memory.read<uintptr_t>((uintptr_t)this + 0x10);
		const uintptr_t internal_list = list + 0x20;

		return memory.read<type>(memory.read<uintptr_t>(internal_list + index * sizeof(type)));

	}

	type operator[](uint32_t index) { return get_value(index); }

};

namespace unity_enums
{

	enum class object_tags : unsigned short
	{
		main_camera = 5, // main camera
		players = 6, // players
		terrain = 20001, // terrain
		corpse = 20009, //Animals & Players
		misc = 20006, //Trash cans, Sleeping Bags, Storage Box, etc
		animal = 20008, // animals
		skydome = 20012, // skydome
		rivermesh = 20014, // rivermesh
		monument = 20015 //Airport, Powerplant, etc
	};

	enum class bone_list : uint32_t
	{
		pelvis = 1,
		l_hip,
		l_knee,
		l_foot,
		l_toe,
		l_ankle_scale,
		penis,
		GenitalCensor,
		GenitalCensor_LOD0,
		Inner_LOD0,
		Inner_LOD01,
		GenitalCensor_LOD1,
		GenitalCensor_LOD2,
		r_hip, r_knee, r_foot, r_toe, r_ankle_scale, spine1, spine1_scale, spine2, spine3, spine4, l_clavicle, l_upperarm, l_forearm, l_hand, l_index1,
		l_index2, l_index3, l_little1, l_little2, l_little3, l_middle1, l_middle2, l_middle3, l_prop, l_ring1, l_ring2, l_ring3, l_thumb1, l_thumb2, l_thumb3,
		IKtarget_righthand_min, IKtarget_righthand_max, l_ulna, neck, head, jaw, eyeTranform, l_eye, l_Eyelid, r_eye, r_Eyelid, r_clavicle,
		r_upperarm, r_forearm, r_hand, r_index1, r_index2, r_index3, r_little1, r_little2, r_little3, r_middle1, r_middle2, r_middle3, r_prop,
		r_ring1, r_ring2, r_ring3, r_thumb1, r_thumb2, r_thumb3, IKtarget_lefthand_min, IKtarget_lefthand_max, r_ulna, l_breast, r_breast, BoobCensor,
		BreastCensor_LOD0, Inner_LOD02, Inner_LOD03, BreastCensor_LOD1, BreastCensor_LOD2
	};

	enum class life_state : uint32_t
	{
		Alive,
		Dead
	};

	enum class player_flags : uint32_t
	{
		Unused1 = 1,
		Unused2 = 2,
		IsAdmin = 4,
		ReceivingSnapshot = 8,
		Sleeping = 16,
		Spectating = 32,
		Wounded = 64,
		IsDeveloper = 128,
		Connected = 256,
		ThirdPersonViewmode = 1024,
		EyesViewmode = 2048,
		ChatMute = 4096,
		NoSprint = 8192,
		Aiming = 16384,
		DisplaySash = 32768,
		Relaxed = 65536,
		SafeZone = 131072,
		ServerFall = 262144,
		Workbench1 = 1048576,
		Workbench2 = 2097152,
		Workbench3 = 4194304,
		UNK = 0x2000
	};

	enum class model_flags : uint32_t
	{
		Ducked = 1,
		Jumped = 2,
		OnGround = 4,
		Sleeping = 8,
		Sprinting = 16,
		OnLadder = 32,
		Flying = 64,
		Aiming = 128,
		Prone = 256,
		Mounted = 512,
		Relaxed = 1024,
	};

	enum class layer : uint32_t {
		Default = 0,
		TransparentFX = 1,
		Ignore_Raycast = 2,
		Reserved1 = 3,
		Water = 4,
		UI = 5,
		Reserved2 = 6,
		Reserved3 = 7,
		Deployed = 8,
		Ragdoll = 9,
		Invisible = 10,
		AI = 11,
		PlayerMovement = 12,
		Vehicle_Detailed = 13,
		Game_Trace = 14,
		Vehicle_World = 15,
		World = 16,
		Player_Server = 17,
		Trigger = 18,
		Player_Model_Rendering = 19,
		Physics_Projectile = 20,
		Construction = 21,
		Construction_Socket = 22,
		Terrain = 23,
		Transparent = 24,
		Clutter = 25,
		Debris = 26,
		Vehicle_Large = 27,
		Prevent_Movement = 28,
		Prevent_Building = 29,
		Tree = 30,
		Unused2 = 31
	};

}