#pragma once
#ifndef _BIG_CORE_BASE_H_
#define _BIG_CORE_BASE_H_

#include <cstdint>
#include <array>
#include <vector>

// length of chunk
const uint64_t CHUNK_LENGTH = 8;

// big magic number
const std::array<char, CHUNK_LENGTH> MAGIC = { 0x42, 0x49, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00 };

const uint64_t DEFAULT_MEMORY_SIZE = 1024ull * 1024ull * 1024ull;

// core chunk ids
enum class CoreChunkIds : uint64_t
{
    NUMBER_OF_IMAGES = 1,
    NUMBER_OF_TILES = 2,
    IMAGE_HEIGHT = 3,
    IMAGE_WIDTH = 4,
    NUMBER_OF_PLANES = 5,
    DATA_ORDER = 6,
    DATA_TYPE = 7,
    DATA = 8,
};

// data type ids
enum class DataTypes : uint8_t
{
    HALF = 1,
    FLOAT = 2,
    DOUBLE = 3,
    CHAR = 4,
    UNSIGNED_CHAR = 5,
    SHORT = 6,
    UNSIGNED_SHORT = 7,
    INT = 8,
    UNSIGNED_INT = 9,
    LONG_LONG = 10,
    UNSIGNED_LONG_LONG = 11,
    BOOL = 12,
};

class BigCoreBase
{
public:

    // getters
    // todo: provide comments of individual methods
    uint64_t getNumberOfImages();
    uint64_t getNumberOfTiles();
    uint64_t getImageHeight();
    uint64_t getImageWidth();
    uint64_t getNumberOfPlanes();
    const std::vector<uint64_t>& getDataOrder();
    const std::vector<uint8_t>& getDataType();

    // Checks whether the container is empty. Returns true until memory is allocated.
    bool empty();

    // Frees memory. In case of output class, enables to change dimensions, data order or data types.
    void clear();

    // Returns true if all data are in memory.
    bool inMemory();

    // Sets maximum possible size of memory that can be used. Does not free memory if already allocated.
    void setMemorySize(uint64_t bytes = DEFAULT_MEMORY_SIZE);

    // Returns total size of the data.
    uint64_t size();

    // Returns size of the used memory.
    uint64_t sizeInMemory();

    // Returns true if all data have the same data type.
    bool uniformDataType();

private:
    // Private constructor.
    BigCoreBase();

protected:
    uint64_t numberOfImages = 0;
    uint64_t numberOfTiles = 1;
    uint64_t imageHeight = 0;
    uint64_t imageWidth = 0;
    uint64_t numberOfPlanes = 1;
    std::vector<uint64_t> dataOrder = { 1, 2, 3, 4, 5 };
    std::vector<uint8_t> dataType = { 2 };
    char *data = nullptr;

    uint64_t dataLength = 0;
    uint64_t memorySize = DEFAULT_MEMORY_SIZE;

    std::vector<uint64_t> outermostEntitiesOffsets;
};

#endif // _BIG_CORE_BASE_H_
