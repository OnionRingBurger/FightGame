#pragma once

#include "Effekseer.h"

class IEffectCacheAcquisition
{
public:
	virtual bool TryGetEffect(std::string, Effekseer::EffectRef& outRef) = 0;
};