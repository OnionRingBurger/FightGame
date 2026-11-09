#include "LoadDataJsonRegistrar.h"

#include <fstream>
#include <Windows.h>

#include "json.hpp"

namespace
{
	std::wstring Utf8ToWide(const std::string& a_utf8)
	{
		if (a_utf8.empty())
		{
			return std::wstring();
		}

		// !!!New!!!
		int size = MultiByteToWideChar(CP_UTF8, 0, a_utf8.c_str(), -1, nullptr, 0);
		if (size <= 0)
		{
			return std::wstring();
		}

		std::wstring result(static_cast<size_t>(size - 1), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, a_utf8.c_str(), -1, &result[0], size);
		return result;
	}

	void RegisterModelsFromJson(LoadDatas& a_loadDatas, const nlohmann::json& a_array)
	{
		if (!a_array.is_array())
		{
			return;
		}

		// !!!New!!!
		for (const nlohmann::json& entry : a_array)
		{
			if (!entry.is_object())
			{
				continue;
			}
			if (!entry.contains("key") || !entry["key"].is_string())
			{
				continue;
			}

			std::string key = entry["key"].get<std::string>();
			float scale = entry.value("scale", 1.0f);
			a_loadDatas.model.push_back({ key, scale });
		}
	}

	void RegisterTexturesFromJson(LoadDatas& a_loadDatas, const nlohmann::json& a_array)
	{
		if (!a_array.is_array())
		{
			return;
		}

		// !!!New!!!
		for (const nlohmann::json& entry : a_array)
		{
			if (entry.is_string())
			{
				a_loadDatas.texture.push_back(entry.get<std::string>());
				continue;
			}

			if (!entry.is_object())
			{
				continue;
			}
			if (!entry.contains("key") || !entry["key"].is_string())
			{
				continue;
			}

			a_loadDatas.texture.push_back(entry["key"].get<std::string>());
		}
	}

	void RegisterEffectsFromJson(LoadDatas& a_loadDatas, const nlohmann::json& a_array)
	{
		if (!a_array.is_array())
		{
			return;
		}

		// !!!New!!!
		for (const nlohmann::json& entry : a_array)
		{
			if (!entry.is_object())
			{
				continue;
			}
			if (!entry.contains("key") || !entry["key"].is_string())
			{
				continue;
			}

			effectLoadData data;
			data.key = entry["key"].get<std::string>();
			data.magnificent = entry.value("magnificent", 1.0f);
			a_loadDatas.effect.push_back(data);
		}
	}

	void RegisterTextFormatsFromJson(LoadDatas& a_loadDatas, const nlohmann::json& a_array)
	{
		if (!a_array.is_array())
		{
			return;
		}

		// !!!New!!!
		for (const nlohmann::json& entry : a_array)
		{
			if (!entry.is_object())
			{
				continue;
			}
			if (!entry.contains("key") || !entry["key"].is_string())
			{
				continue;
			}

			textFormatData data;
			data.key = entry["key"].get<std::string>();
			data.useFile = entry.value("useFile", false);
			data.font = Utf8ToWide(entry.value("font", std::string()));
			data.fontFile = Utf8ToWide(entry.value("fontFile", std::string()));
			data.size = entry.value("size", 0.0f);
			a_loadDatas.textFormat.push_back(data);
		}
	}

	void RegisterTextUIsFromJson(LoadDatas& a_loadDatas, const nlohmann::json& a_array)
	{
		if (!a_array.is_array())
		{
			return;
		}

		// !!!New!!!
		for (const nlohmann::json& entry : a_array)
		{
			if (!entry.is_object())
			{
				continue;
			}
			if (!entry.contains("key") || !entry["key"].is_string())
			{
				continue;
			}

			textUILoadData data;
			data.key = entry["key"].get<std::string>();
			data.text = Utf8ToWide(entry.value("text", std::string()));
			data.formatKey = entry.value("formatKey", std::string());
			data.width = entry.value("width", 0.0f);
			data.height = entry.value("height", 0.0f);
			a_loadDatas.textUI.push_back(data);
		}
	}
}

LoadDataJsonRegistrar::LoadDataJsonRegistrar(LoadDatas& a_loadDatas)
	: loadDatas(a_loadDatas)
{
}

bool LoadDataJsonRegistrar::RegisterFromManifest(const LoadManifestPaths& a_paths)
{
	if (a_paths.path.empty())
	{
		return false;
	}

	std::ifstream stream(a_paths.path);
	if (!stream.is_open())
	{
		return false;
	}

	nlohmann::json json;
	stream >> json;
	if (!json.is_object())
	{
		return false;
	}

	// !!!New!!!
	if (json.contains("models"))
	{
		RegisterModelsFromJson(loadDatas, json["models"]);
	}
	if (json.contains("textures"))
	{
		RegisterTexturesFromJson(loadDatas, json["textures"]);
	}
	if (json.contains("effects"))
	{
		RegisterEffectsFromJson(loadDatas, json["effects"]);
	}
	if (json.contains("textFormats"))
	{
		RegisterTextFormatsFromJson(loadDatas, json["textFormats"]);
	}
	if (json.contains("textUIs"))
	{
		RegisterTextUIsFromJson(loadDatas, json["textUIs"]);
	}
	if (json.contains("jsonPrefab") && json["jsonPrefab"].is_string())
	{
		loadDatas.jsonPrefab.push_back({ json["jsonPrefab"].get<std::string>() });
	}

	return true;
}
