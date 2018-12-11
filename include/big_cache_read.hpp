#pragma once
#ifndef _BIG_CACHE_READ_H_
#define _BIG_CACHE_READ_H_

#include "big_consts.hpp"

#include <cstdint>
#include <fstream>
#include <vector>
#include <list>
#include <memory>
#include <numeric>

namespace big
{

    class BigCacheRead
    {
    private:

        struct Entity
        {
            std::shared_ptr<char> data;
            DataTypes dataType;
            std::shared_ptr<uint8_t> data_uint8_t;
            std::shared_ptr<uint16_t> data_uint16_t;
            uint64_t dataSize = 0;
            std::list<uint64_t>::iterator it;
            //bool dirty = false;
        };

        std::vector<Entity> entities;           // entities currently stored in a memory
        std::list<uint64_t> lru_list;           // list of least recently used entities, starting with the least recently used and ending with the most recently used entity

        uint64_t currentSize = 0;               // size of the used memory
        uint64_t maxSize = DEFAULT_CACHE_SIZE;  // the maximal size of usable memory

        std::vector<uint64_t>& entitySizes = std::vector<uint64_t>();   // sizes of the entities
        std::vector<uint64_t>& dataPositions = std::vector<uint64_t>(); // positions of the entities in a file

    public:

        void initialize(std::vector<uint64_t>& entitySizes, std::vector<uint64_t>& dataPositions) { this->entitySizes = entitySizes; this->dataPositions = dataPositions; entities.resize(entitySizes.size()); }

        void setSize(uint64_t maxSize) { this->maxSize = maxSize; }

        uint64_t getSize() { return currentSize; }

        void load(std::ifstream &file);

        void clear();

        void shrink();

        // save dirty entities from the cache to a file
        void flush(std::ofstream &file);

        std::shared_ptr<char> getEntity(uint64_t index, std::ifstream &file);

        void addEntity(std::shared_ptr<char> data, uint64_t index, std::ofstream &file);

        template<typename T>
        T getElement(uint64_t entityID, uint64_t index, std::ifstream &file);

        template<typename T>
        void addElement(const T& value, uint64_t entityID, uint64_t index, std::ofstream &file);

    private:

        std::shared_ptr<char> insert(const uint64_t& index, std::ifstream &file);

        void save(std::shared_ptr<char> data, uint64_t index, std::ofstream &file);
    };

    template<typename T>
    T BigCacheRead::getElement(uint64_t entityID, uint64_t index, std::ifstream &file)
    {
        Entity& entity = entities[entityID];
        //if (entity.data == nullptr) {
        //    if (entitySizes[entityID] <= maxSize) {
        //        return reinterpret_cast<T*>(insert(entityID, file).get())[index];
        //    }
        //    else {
        //        // read data directly
        //        file.seekg(dataPositions[entityID] + index * sizeof(T));
        //        T val;
        //        file.read(reinterpret_cast<char*>(&val), sizeof(T));
        //        return val;
        //    }
        //}
        lru_list.splice(lru_list.end(), lru_list, entity.it);

        switch (entity.dataType)
        {
        case DataTypes::UINT8_T:
            uint8_t& val = entity.data_uint8_t.get()[index];
            if (std::is_same<T, uint8_t>::value) return val;
            else if (std::is_same<T, uint16_t>::value) return val * 256 + 127;
            else if (std::is_same<T, uint32_t>::value) return val * 256 * 65536 + 8388607;
            else if (std::is_same<T, uint32_t>::value) return val * 256 * 65536 + 8388607;
            else if (std::is_same<T, uint64_t>::value) return static_cast<uint64_t>(val) * 256ull * 65536ull * 4294967296ull + 36028797018963967ull;
            break;
        }


        return reinterpret_cast<T*>(entity.data.get())[index];
    }

}

#endif // _BIG_CACHE_READ_H_
