#pragma once
#ifndef _BIG_CORE_READ_H_
#define _BIG_CORE_READ_H_

#include "big_core_read_info.hpp"
#include "big_cache_read.hpp"

#include <fstream>
#include <array>
#include <memory>

namespace big
{
    class BigCoreRead : public BigCoreReadInfo
    {
    public:

        // Constructor. Opens file, reads parameters of the container, sets size of cache and reads all data into cache if requested and possible.
        BigCoreRead(const std::string& fileName, bool loadToCache = false, uint64_t cacheSizeBytes = DEFAULT_CACHE_SIZE);

        // Copy constructor forbidden.
        BigCoreRead(const BigCoreRead &) = delete;

        // Move constructor.
        BigCoreRead(BigCoreRead &&other) = default;

        // Destructor.
        ~BigCoreRead() = default;

        // Copy-assignment operator forbidden.
        BigCoreRead &operator=(const BigCoreRead &) = delete;

        // Move-assignment operator.
        BigCoreRead &operator=(BigCoreRead &&rhs) = default;



        // Returns element in the given image at given row, column and (color) plane. Does not check bounds.
        template<typename T>
        T operator() (uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0);

        // Returns element in the given image at given row, column and (color) plane. Checks bounds.
        template<typename T>
        T at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0);

        // Returns pointer to the entity specified by its index.
        std::shared_ptr<const char> operator[] (uint64_t index);

        // Returns the entity specified by its index. Values are casted to the specified data type.
        template<typename T>
        std::shared_ptr<const T> getEntity(uint64_t index);

        // Returns the image specified by its number. Values are casted to the specified data type.
        template<typename T>
        std::shared_ptr<T> getImage(uint64_t imageNum);



        // Loads all data into cache, if possible. Does nothing if data are bigger than cache size.
        void loadToCache() { cache.load(file); }

        // Sets maximal possible size of cache that can be used and loads all data into cache, if possible.
        // Does nothing if data are bigger than cache size.
        void loadToCache(uint64_t bytes) { cache.setSize(bytes); cache.load(file); }

        // Releases all memory allocated by cache.
        void clearCache() { cache.clear(); }

        // Reduces memory usage by freeing least recently used entities.
        void shrinkCache(uint64_t bytes) { cache.setSize(bytes); cache.shrink(); }

        // Sets maximal possible size of cache that can be used. Does not free memory if already allocated.
        void setCacheSize(uint64_t bytes) { cache.setSize(bytes); }

        // Returns size of the used memory.
        uint64_t getCacheSize() { return cache.getSize(); }

        // Returns true if all data are in cache.
        bool isAllInCache() { return cache.getSize() == dataSize; }

    protected:
        BigCacheRead cache;

    };
}

#endif // _BIG_CORE_READ_H_
