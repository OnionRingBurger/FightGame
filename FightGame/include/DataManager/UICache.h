#pragma once

#include <unordered_map>
#include <wrl/client.h>
#include <string>
#include <memory>
#include "IUICacheAcquisition.h"
#include "GameTexture.h"


using Microsoft::WRL::ComPtr;

class UICache : public IUICacheAcquisition
{
public:
	UICache();
	~UICache() = default;

	void RegisterTexture(std::string a_key, std::shared_ptr<Texture> a_texture);
	std::shared_ptr<Texture> GetTexture(std::string a_key) const override;
	ComPtr<IDWriteTextFormat> GetTextFormat(std::string a_key) const override;

	void RegisterTextFormat(std::string a_key, Microsoft::WRL::ComPtr<IDWriteTextFormat> a_format);
	

private:
	
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureCache;
	std::unordered_map <std::string, ComPtr<IDWriteTextFormat>> m_textFormatCache;


};

