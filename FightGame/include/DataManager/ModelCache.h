#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "Model.h"
#include "IModelCacheAcquisition.h"

class ModelCache : public IModelCacheAcquisition
{
public:
	ModelCache();
	~ModelCache() = default;
	std::shared_ptr<Model> GetModel(std::string key)const override;

	void RegisterModel(std::string, std::shared_ptr<Model> a_model);

private:
	std::unordered_map<std::string, std::shared_ptr<Model>> m_cache;

};
