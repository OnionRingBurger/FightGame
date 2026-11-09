#pragma once
#include <unordered_map>
#include <string>

#include "IEffectCacheAcquisition.h"
#include "Effekseer.h"

class EffectCache : public IEffectCacheAcquisition
{
public:
	EffectCache();
	~EffectCache();

	bool TryGetEffect(std::string, Effekseer::EffectRef& outRef) override;

	void RegisterEffect(std::string key, Effekseer::EffectRef effectRef);

private:
	std::unordered_map<std::string, Effekseer::EffectRef> effectCache;
};