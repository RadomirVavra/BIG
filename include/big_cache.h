#pragma once
#ifndef _BIG_BUFFER_H_
#define _BIG_BUFFER_H_

#include <cstdint>
#include <fstream>
#include <vector>
#include <list>

class BigCache
{
private:

    struct Entity
    {
        char* data = nullptr; // todo: use smart pointer
        uint64_t dataSize = 0;
        std::list<uint64_t>::iterator it;
    };

    std::vector<Entity> entities;   // entities currently stored in a memory
    std::list<uint64_t> lru_list;   // list of least recently used entities, starting with the least recently used and ending with the most recently used entity

    uint64_t memorySize = 0;
    uint64_t maxMemorySize = 0;

    uint64_t dataPosition;                  // position of the data array in a file
    uint64_t entityBaseSize;                // size of one entity without data type considered
    std::vector<uint64_t> offsets;          // offsets of the outermost entities
    std::vector<uint64_t> entityTypeSizes;  // sizes of data types of the outermost entities

public:

    void initialize(const uint64_t& maxMemorySize, const uint64_t& dataPosition, const uint64_t& entityBaseSize, const std::vector<uint64_t>& offsets, const std::vector<uint64_t>& entityTypeSizes);

    char *getEntity(uint64_t index, std::ifstream &file);

private:

    void insert(const uint64_t& index, Entity& e);

    void evict();

    Entity readEntity(uint64_t index, std::ifstream &file);
};

#endif // _BIG_BUFFER_H_
