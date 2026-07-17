#pragma once

#include <string>
#include <vector>
#include <memory>
#include <atomic>

#include "Model.h"
#include "Texture.h"
#include "Sound.h"
#include "DebugConsole.h"

using modelLoadData = std::pair<std::string, float>;

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

struct SoundLoadJob
{
	std::string path;

	SoundLoadJob(std::string a_path)
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

void LoadSound();

ModelLoadResult LoadModel(const ModelLoadJob& job);
TextureLoadResult LoadTexture(const TextureLoadJob& job);