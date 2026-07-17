#pragma once
#include <memory>
#include <functional>

#include "Input.h"
#include "IModelCacheAcquisition.h"
#include "IUICacheAcquisition.h"
#include "TrailEffect.h"

struct SystemContext
{
	const Input& input;
	IModelCacheAcquisition& modelCache;
	IUICacheAcquisition& uiCache;
	std::unique_ptr<TrailEffect> trailEffect;
	float deltaTime;
	float effectStepTime;
	std::function<void(int)> tutorialRequest;

	SystemContext(IModelCacheAcquisition& a_modelCache, IUICacheAcquisition& a_uiCache, std::function<void(int)> a_tutorialRequest, const Input& a_input)
		: deltaTime(0.0f)
		, effectStepTime(0.0f)
		, modelCache(a_modelCache)
		, uiCache(a_uiCache)
		, trailEffect(std::make_unique<TrailEffect>())
		, tutorialRequest(a_tutorialRequest)
		, input(a_input)
	{
	}
};
