#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "IUICacheAcquisition.h"
#include "GameTexture.h"

class UICache : public IUICacheAcquisition
{
public:
	UICache();
	~UICache() = default;

	void RegisterModel(std::string a_key, std::shared_ptr<Texture> a_texture);
	std::shared_ptr<Texture> GetTexture(std::string a_key) const override;

private:
	
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureCache;


};

