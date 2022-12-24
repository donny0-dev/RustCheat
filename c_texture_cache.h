#pragma once

#include "Overlay/Overlay.h"

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")

#include <string>
#include <vector>

// FUCK STD::TUPLE, REBEL!!!!

class c_texture
{
public:
	IDirect3DTexture9* texture = nullptr;
	std::string item_name = "";
};

class c_texture_cache
{
private:
	std::vector<c_texture> texture_cache{};

	IDirect3DTexture9* create_texture(LPDIRECT3DDEVICE9 device, std::string name)
	{

		std::string image_path = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Rust\\Bundles\\items\\" + name; // todo: make path dynamic
		auto* tex = LPDIRECT3DTEXTURE9();

		if (D3DXCreateTextureFromFileA(device, image_path.c_str(), &tex) != D3D_OK)
			return nullptr;

		return tex;

	}

public:

	c_texture get_texture(std::string item_name)
	{

		for (auto texture : texture_cache)
			if (texture.item_name == item_name)
				return texture; // check if the texture's already cached

		c_texture ret;
		ret.item_name = item_name;
		ret.texture = create_texture(g_pd3dDevice, item_name + ".png");

		if (!ret.texture)
			return ret; // this issue sorts itself out

		texture_cache.push_back(ret); // put the new texture into the cache

		return ret; // return the texture

	}

}; inline c_texture_cache texture_cache;