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
            std::list<uint64_t>::iterator it;
        };

        std::vector<Entity> entities;           // entities currently stored in a memory
        std::list<uint64_t> lru_list;           // list of least recently used entities, starting with the least recently used and ending with the most recently used entity

        uint64_t currentSize = 0;               // size of the used memory
        uint64_t maxSize = DEFAULT_CACHE_SIZE;  // the maximal size of usable memory

        std::vector<uint64_t>& entitySizes;// = std::vector<uint64_t>();    // sizes of the entities
        std::vector<uint64_t>& dataPositions;// = std::vector<uint64_t>();  // positions of the entities in a file
        std::vector<DataTypes>& dataTypes;
        std::ifstream& file;                        // attached file

    public:

        BigCacheRead(std::ifstream &file, std::vector<uint64_t> &entitySizes, std::vector<uint64_t> &dataPositions, std::vector<DataTypes> &dataTypes)
            : file(file), entitySizes(entitySizes), dataPositions(dataPositions), dataTypes(dataTypes)
        {
            entities.resize(entitySizes.size());
        }

        void setSize(uint64_t maxSize) { this->maxSize = maxSize; }

        uint64_t getSize() { return currentSize; }

        void load(std::ifstream &file)
        {
            uint64_t totalSize = 0;
            for (const auto & entitySize : entitySizes) totalSize += entitySize;
            if (totalSize <= maxSize) {
                for (uint64_t index = 0; index != entities.size(); ++index) {
                    if (entities[index].data != nullptr) continue;
                    pull(index);
                }
            }
        }

        void clear()
        {
            while (!lru_list.empty()) pop();
        }

        void shrink()
        {
            while (currentSize > maxSize) pop();
        }

        // Returns pointer to the entity specified by its index.
        std::shared_ptr<const char> operator[] (uint64_t index)
        {
            Entity& entity = entities[index];
            if (entity.data == nullptr) {
                if (entitySizes[index] <= maxSize) insert(index);
                else {
                    std::shared_ptr<char> data = std::shared_ptr<char>(new char[entitySizes[index]], [](char *p) { delete[] p; });
                    file.seekg(dataPositions[index]);
                    file.read(data.get(), entitySizes[index]);
                    return data;
                }
            }
            else {
                lru_list.splice(lru_list.end(), lru_list, entity.it);
            }
            return entity.data;
        }

        template<typename T>
        std::vector<T> getEntity(uint64_t index)
        {
            Entity& entity = entities[index];
            std::shared_ptr<char> data = entity.data;
            if (data == nullptr) {
                if (entitySizes[index] <= maxSize) {
                    insert(index);
                    data = entity.data;
                }
                else {
                    data = std::shared_ptr<char>(new char[entitySizes[index]], [](char *p) { delete[] p; });
                    file.seekg(dataPositions[index]);
                    file.read(data.get(), entitySizes[index]);
                }
            }
            else {
                lru_list.splice(lru_list.end(), lru_list, entity.it);
            }

            std::vector<T> vec;

            switch (dataTypes[index])
            {
                case DataTypes::UINT8_T:
                {
                    vec.reserve(entitySizes[index]);
                    for (uint64_t i = 0; i != entitySizes[index]; ++i) {
                        vec.push_back(convert<T, uint8_t>(reinterpret_cast<uint8_t*>(data.get())[i]));
                    }
                    return vec;
                }
                case DataTypes::UINT16_T:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, uint16_t>(reinterpret_cast<uint16_t*>(data.get())[i]));
                    }
                    return vec;
                }
                case DataTypes::UINT32_T:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, uint32_t>(reinterpret_cast<uint32_t*>(data.get())[i]));
                    }
                    return vec;
                }
                case DataTypes::UINT64_T:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, uint64_t>(reinterpret_cast<uint64_t*>(data.get())[i]));
                    }
                    return vec;
                }
                case DataTypes::INT8_T:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, int8_t>(reinterpret_cast<int8_t*>(data.get())[i]));
                    }
                    return vec;
                }
                case DataTypes::INT16_T:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, int16_t>(reinterpret_cast<int16_t*>(data.get())[i]));
                    }
                    return vec;
                }
                case DataTypes::INT32_T:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, int32_t>(reinterpret_cast<int32_t*>(data.get())[i]));
                    }
                    return vec;
                }
                case DataTypes::INT64_T:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, int64_t>(reinterpret_cast<int64_t*>(data.get())[i]));
                    }
                    return vec;
                }
               /* case DataTypes::HALF:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, half>(reinterpret_cast<half*>(data.get())[i]));
                    }
                    return vec;
                }*/
                case DataTypes::FLOAT:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, float>(reinterpret_cast<float*>(data.get())[i]));
                    }
                    return vec;
                }
                case DataTypes::DOUBLE:
                {
                    vec.reserve(entitySizes[index] / 2);
                    for (uint64_t i = 0; i != entitySizes[index] / 2; ++i) {
                        vec.push_back(convert<T, double>(reinterpret_cast<double*>(data.get())[i]));
                    }
                    return vec;
                }
        
        
            }

        
        
            return vec;
        }

        template<typename T>
        T getElement(uint64_t entityID, uint64_t index);


        template<typename T>
        T getElementFromMemory(uint64_t entityID, uint64_t index);

    private:

        template<typename Tdst, typename Tsrc>
        Tdst convert(Tsrc value);

        template<typename T>
        T getElementFromFile(uint64_t entityID, uint64_t index);

        void pop()
        {
            entities[lru_list.front()].data.reset();
            currentSize -= entitySizes[lru_list.front()];
            lru_list.pop_front();
        }

        void pull(uint64_t index)
        {
            entities[index].data = std::shared_ptr<char>(new char[entitySizes[index]], [](char *p) { delete[] p; });
            file.seekg(dataPositions[index]);
            file.read(entities[index].data.get(), entitySizes[index]);
            currentSize += entitySizes[index];
            entities[index].it = lru_list.insert(lru_list.end(), index);
        }

        void insert(const uint64_t& index)
        {
            while (currentSize + entitySizes[index] > maxSize) pop();
            pull(index);
        }
    };
}

#endif // _BIG_CACHE_READ_H_
