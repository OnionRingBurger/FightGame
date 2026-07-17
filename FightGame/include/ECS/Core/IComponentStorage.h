#pragma once

#include "ECSTypes.h"
#include <vector>

class IComponentStorage
{
public:
	IComponentStorage(){};
	virtual ~IComponentStorage() = default;

	virtual bool Has(const Entity &) = 0;

	virtual void Delete(Entity) = 0;

	virtual std::vector<Entity>& GetHaveEntities() = 0;

	virtual std::vector<Entity>& GetchangedEntities() = 0;


};
