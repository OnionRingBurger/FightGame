#pragma once

#include <string>
#include <memory>
#include <wrl/client.h>

#include "GameTexture.h"

using Microsoft::WRL::ComPtr;

class IUICacheAcquisition
{
public:
	virtual std::shared_ptr<Texture> GetTexture(std::string a_key)const = 0;
	virtual ComPtr<IDWriteTextFormat> GetTextFormat(std::string a_key) const = 0;

};