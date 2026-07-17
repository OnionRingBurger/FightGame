#pragma once
#include <string>
#include <memory>

#include "Model.h"

class IModelCacheAcquisition
{
public:
	virtual std::shared_ptr<Model> GetModel(std::string key)const = 0;
};