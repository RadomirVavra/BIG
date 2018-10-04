#include "../include/big_cache.h"

void BigCache::initialize(const uint64_t& maxMemorySize, const uint64_t& dataPosition, const uint64_t& entityBaseSize, const std::vector<uint64_t>& offsets, const std::vector<uint64_t>& entityTypeSizes)
{
    this->maxMemorySize = maxMemorySize;
    this->dataPosition = dataPosition;
    this->entityBaseSize = entityBaseSize;
    this->offsets = offsets;
    this->entityTypeSizes = entityTypeSizes;
}

char *BigCache::getEntity(uint64_t index, std::ifstream &file)
{
    Entity& entity = entities[index];
    if (entity.data == nullptr) {
        // todo: re-use memory if possible
        Entity e = readEntity(index, file);
        insert(index, e);
        return e.data;
    }

    lru_list.splice(lru_list.end(), lru_list, entity.it);

    return entity.data;
}

void BigCache::insert(const uint64_t& index, Entity& e)
{
    while (memorySize + e.dataSize > maxMemorySize && memorySize > 0) evict();
    e.it = lru_list.insert(lru_list.end(), index);
    entities[index] = e;
}

void BigCache::evict()
{
    Entity& entity = entities[lru_list.front()];
    lru_list.pop_front();
    delete[] entity.data;
    entity.data = nullptr;
    memorySize -= entity.dataSize;
    entity.dataSize = 0;
}

BigCache::Entity BigCache::readEntity(uint64_t index, std::ifstream &file)
{
    Entity entity;
    file.seekg(dataPosition + offsets[index]);
    entity.dataSize = entityBaseSize * entityTypeSizes[index];
    entity.data = new char[entity.dataSize];
    file.read(entity.data, entity.dataSize);
    return entity;
}
