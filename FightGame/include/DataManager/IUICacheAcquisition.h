#pragma once

#include <string>
#include <memory>

#include "GameTexture.h"

class IUICacheAcquisition
{
public:
	virtual std::shared_ptr<Texture> GetTexture(std::string a_key)const = 0;

};