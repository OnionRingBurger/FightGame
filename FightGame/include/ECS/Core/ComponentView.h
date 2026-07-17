#pragma once

#include "IComponentStorage.h"
#include <memory>
#include <vector>


class ComponentView
{
public:


	ComponentView(const std::vector<std::vector<Entity>>& entities);
	ComponentView(const std::vector<std::vector<Entity>>& entities, const std::vector<std::vector<Entity>>& exclusionEntities);
	~ComponentView() = default;

	using iterator = std::vector<Entity>::const_iterator;



	iterator begin();

	iterator end();

private:

	std::vector<Entity> entities;

};


