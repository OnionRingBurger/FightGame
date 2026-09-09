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
// TODO ãÍì˜ÇÃçÙ
#include "UICache.h"
#include <wrl/client.h>

using modelLoadData = std::pair<std::string, float>;
using Microsoft::WRL::ComPtr;

struct effectLoadData
{
	std::string key;
	float magnificent;
};

struct textFormatData
{
	std::string key;
	bool useFile;
	std::wstring font;
	std::wstring fontFile;
	float size;
};

struct textUILoadData
{
	std::string key;
	std::wstring text;
	std::string formatKey;
	float width;
	float height;
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

struct TextFormatLoadJob
{
	std::shared_ptr<std::atomic_bool> const endFlagPointer;

	std::string key;
	bool useFile;
	std::wstring font;
	std::wstring fontFile;
	float size;

	TextFormatLoadJob(
		std::string a_key,
		bool a_useFile,
		std::wstring a_font,
		std::wstring a_fontFile,
		float a_size,
		std::shared_ptr<std::atomic_bool> const a_endFlagPointer)
		: key(a_key)
		, useFile(a_useFile)
		, font(a_font)
		, fontFile(a_fontFile)
		, size(a_size)
		, endFlagPointer(a_endFlagPointer)
	{
	};
};

struct TextUILoadJob
{
	std::shared_ptr<std::atomic_bool> const endFlagPointer;

	std::string key;
	std::wstring text;
	std::string formatKey;
	float width;
	float height;

	TextUILoadJob(
		std::string a_key,
		std::wstring a_text,
		std::string a_formatKey,
		float a_width,
		float a_height,
		std::shared_ptr<std::atomic_bool> const a_endFlagPointer
	)
		: key(a_key)
		, text(a_text)
		, formatKey(a_formatKey)
		, width(a_width)
		, height(a_height)
		, endFlagPointer(a_endFlagPointer)
	{
	}
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

// ñ¢é¿ëï
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

struct TextFormatLoadResult
{
	std::string key;
	ComPtr<IDWriteTextFormat> format;

	TextFormatLoadResult()
		: key("")
		, format(nullptr)
	{

	}

	TextFormatLoadResult(std::string a_key, ComPtr<IDWriteTextFormat> a_format)
		: key(a_key)
		, format(a_format)
	{
	}
};


// TextureLoadResultÇ∆ìØÇ∂ÇæÇ™ÅAïœçXÇµÇ‚Ç∑Ç¢ÇΩÇﬂï ç\ë¢ëÃÇ÷ï™ÇØÇƒÇÈ
struct TextUILoadResult
{
	std::string key;
	std::shared_ptr<Texture> data;

	TextUILoadResult(std::string a_key, std::shared_ptr<Texture> a_data)
		: key(a_key)
		, data(a_data)
	{
	}

	TextUILoadResult()
		: key()
		, data(nullptr)
	{
	}

};

void LoadSound();

ModelLoadResult LoadModel(const ModelLoadJob& job);
TextureLoadResult LoadTexture(const TextureLoadJob& job);
EffectLoadResult LoadEffect(const EffectLoadJob& job);
TextFormatLoadResult LoadFormat(const TextFormatLoadJob& a_job);
TextUILoadResult LoadTextUI(const TextUILoadJob& a_job, const IUICacheAcquisition& a_cache);