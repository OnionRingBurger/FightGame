#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include "IComponentStorage.h"
#include "Types.h"


class HasView
{
public:
	
	HasView(Entity, std::unordered_map<TypeID, std::shared_ptr<IComponentStorage>>&);
	HasView();
	~HasView() = default;
	
	using iterator = std::vector<TypeID>::iterator;

	iterator begin();
	iterator end();

private:
	std::vector<TypeID> storage;
};

