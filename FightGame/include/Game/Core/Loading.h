#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>

#include "Model.h"
#include "Texture.h"
#include "Sound.h"
#include "Effekseer.h"
#include "DebugConsole.h"

using modelLoadData = std::pair<std::string, float>;

struct effectLoadData
{
	std::string key;
	float magnificent;
};

struct ModelLoadJob
{
	std::string key;
	float scale;
	std::shared_ptr<std::atomic_bool> const endFlagPointer;

	ModelLoadJob(std::string a_key, float a_scale = 1.0f, std::shared_ptr<std::atomic_bool> const a_endFlagPointer = nullptr)
		: key(a_key)
		, scale(a_scale)
		, endFlagPointer(a_endFlagPointer)
	{
		if (endFlagPointer)
		{
			endFlagPointer->store(false);
		}
	};

};

struct TextureLoadJob
{
	std::string key;
	std::shared_ptr<std::atomic_bool> const endFlagPointer;

	TextureLoadJob(std::string a_key, std::shared_ptr<std::atomic_bool> const a_endFlagPointer = nullptr)
		: key(a_key)
		, endFlagPointer(a_endFlagPointer)
	{
		if (endFlagPointer)
		{
			endFlagPointer->store(false);
		}
	};

};

struct EffectLoadJob
{
	std::string key;
	float magnificent;
	std::shared_ptr<std::atomic_bool> const endFlagPointer;

	EffectLoadJob(std::string a_key, float a_magnificent, std::shared_ptr<std::atomic_bool> const a_endFlagPointer = nullptr)
		: key(a_key)
		, magnificent(a_magnificent)
		, endFlagPointer(a_endFlagPointer)
	{
		if (endFlagPointer)
		{
			endFlagPointer->store(false);
		}
	}
	
};

struct SoundLoadJob
{
	std::string path;
	std::shared_ptr<std::atomic_bool> const endFlagPointer;

	SoundLoadJob(
		std::string a_path,
		std::shared_ptr<std::atomic_bool> const endFlagPointer)
		: path(a_path)
	{
	};
};

struct ModelLoadResult
{
	std::string key;
	std::shared_ptr<Model> data;

	ModelLoadResult()
		: key()
		, data(nullptr)
	{

	}

	ModelLoadResult(std::string a_key, std::shared_ptr<Model> a_data)
		: key(a_key)
		, data(a_data)
	{
	}
};

// –¢ŽÀ‘•
struct SoundLoadResult
{
	// std::vector<std::shared_ptr<Model>> data;
};

struct TextureLoadResult
{
	std::string key;
	std::shared_ptr<Texture> data;

	TextureLoadResult()
		: key()
		, data(nullptr)
	{
	}

	TextureLoadResult(std::string a_key, std::shared_ptr<Texture> a_data)
		: key(a_key)
		, data(a_data)
	{
	}
};

struct EffectLoadResult
{
	std::string key;
	Effekseer::EffectRef effect;

	EffectLoadResult()
		: key("")
		, effect(nullptr)
	{

	}

	EffectLoadResult(std::string a_key, Effekseer::EffectRef a_effect)
		: key(a_key)
		, effect(a_effect)
	{
	}
};

void LoadSound();

ModelLoadResult LoadModel(const ModelLoadJob& job);
TextureLoadResult LoadTexture(const TextureLoadJob& job);
EffectLoadResult LoadEffect(const EffectLoadJob& job);