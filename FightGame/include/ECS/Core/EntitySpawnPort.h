#ifndef ___ENTITY_SPAWN_PORT_H___
#define ___ENTITY_SPAWN_PORT_H___

#include <vector>
#include "ECSTypes.h"
#include <any>
#include <unordered_map>

class Chunk;


class EntitySpawnPort
{
public:
	EntitySpawnPort() = default;
	~EntitySpawnPort() = default;
	std::vector<Entity> AddEntities(Chunk& a_chunk, const EntitySpawnRequest& a_request);

};

#endif
