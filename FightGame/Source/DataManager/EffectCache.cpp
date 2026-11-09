#include "EffectCache.h"

EffectCache::EffectCache()
{
}

EffectCache::~EffectCache()
{
}

bool EffectCache::TryGetEffect(std::string key, Effekseer::EffectRef& outRef)
{
	if (effectCache.find(key) == effectCache.end()) return false;
	outRef = effectCache.at(key);
	return true;
}

void EffectCache::RegisterEffect(std::string key ,Effekseer::EffectRef effectRef)
{
	if (!effectRef) return;
	effectCache.insert({ key, effectRef });
}
