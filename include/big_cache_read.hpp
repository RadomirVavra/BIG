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

		BigCacheRead(std::ifstream &file, std::vector<uint64_t> &entitySizes, std::vector<uint64_t> &dataPositions, std::vector<DataTypes> &dataTypes);

		//set max cachce size
		void setSize(uint64_t maxSize);

		//return size of used memoryy
		uint64_t getSize();

		//load data until cache isn't full 
		void load(std::ifstream &file);
        
		//clear list of recently used entities
		void clear();
        
		//shrink cache data to max size
		void shrink();
        
        // Returns pointer to the entity specified by its index.
		std::shared_ptr<const char> operator[] (uint64_t index);
        
		// Returns pointer to the entity specified by its index.
        template<typename T>
		std::vector<T> getEntity(uint64_t index);
        
		// return element by entity ID and her index
        template<typename T>
        T getElement(uint64_t entityID, uint64_t index);

		//return element from cache by entity ID and her index
        template<typename T>
        T getElementFromMemory(uint64_t entityID, uint64_t index);

    private:

		//coverts datatypes
        template<typename Tdst, typename Tsrc>
        Tdst convert(Tsrc value);

		//load and save element from file to cache, select by entity ID and her index
        template<typename T>
        T getElementFromFile(uint64_t entityID, uint64_t index);

		//delete least used entity
		void pop();
        

		void pull(uint64_t index);

		void insert(const uint64_t& index);
        
    };
}

#endif // _BIG_CACHE_READ_H_
