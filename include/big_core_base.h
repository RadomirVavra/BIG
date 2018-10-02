#pragma once
#ifndef _BIG_CORE_BASE_H_
#define _BIG_CORE_BASE_H_

#include <cstdint>
#include <array>
#include <map>
#include <vector>

// length of chunk
const uint64_t CHUNK_LENGTH = 8;

// big magic number
const std::array<char, CHUNK_LENGTH> MAGIC = { 0x42, 0x49, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00 };

// default size of usable memory
const uint64_t DEFAULT_MEMORY_SIZE = 1024ull * 1024ull * 1024ull;

// core chunk ids
enum class CoreChunkIds : uint64_t
{
    NUMBER_OF_IMAGES = 1,
    IMAGE_HEIGHT = 2,
    IMAGE_WIDTH = 3,
    NUMBER_OF_PLANES = 4,
    DATA_ORDER = 5,
    DATA_TYPE = 6,
    DATA = 7,
};

// data order ids
enum class DataOrderIds : uint64_t
{
    NUMBER_OF_IMAGES = 1,
    IMAGE_HEIGHT = 2,
    IMAGE_WIDTH = 3,
    NUMBER_OF_PLANES = 4,
};

// data type ids
enum class DataTypes : uint64_t
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

// size of data types
const std::vector<uint64_t> typeSizes = { 0, 2, 4, 8, 1, 1, 2, 2, 4, 4, 8, 8, 1 };

class BigCoreBase
{
protected:

    // Default constructor.
    BigCoreBase() {}

    // Copy constructor forbidden.
    BigCoreBase(const BigCoreBase &) = delete;

    // Assignment operator forbidden.
    BigCoreBase &operator=(const BigCoreBase &) = delete;

public:

    // Destructor.
    ~BigCoreBase();

    // Returns number of contained images
    uint64_t getNumberOfImages() { return numberOfImages; }

    // Returns height of images
    uint64_t getImageHeight() { return imageHeight; }

    // Returns width of images
    uint64_t getImageWidth() { return imageWidth; }

    // Returns number of (color) planes
    uint64_t getNumberOfPlanes() { return numberOfPlanes; }

    // Returns order in which the data are serialized into memory
    const std::vector<DataOrderIds>& getDataOrder() { return dataOrder; }

    // Returns type(s) of data
    const std::vector<DataTypes>& getDataType() { return dataType; }

    // Checks whether the container is empty. Returns true until memory is allocated.
    bool isEmpty() { return dataSize == 0; }

    // Frees memory. In case of output class, enables to change dimensions, data order or data types once again.
    void clear();

    // Returns true if all data are in memory.
    bool isInMemory() { return _data != nullptr && memorySize == dataSize; }

    // Sets maximum possible size of memory that can be used. Does not free memory if already allocated.
    void setMaxMemorySize(uint64_t bytes = DEFAULT_MEMORY_SIZE) { maxMemorySize = bytes; }

    // Returns total size of the data.
    uint64_t size() { return dataSize; }

    // Returns size of the used memory.
    uint64_t sizeInMemory() { return memorySize; }

protected:

    // Initializes supporting data structures.
    void setSupportingStructures();

    // Do not call directly, use setSupportingStructures() method.
    void setDimensions();

    // Do not call directly, use setSupportingStructures() method.
    void setEntityTypeSizes();

    // Do not call directly, use setSupportingStructures() method.
    void setSubSizes();

    // Do not call directly, use setSupportingStructures() method.
    void setDataSize();

    // Do not call directly, use setSupportingStructures() method.
    void setOffsets();

    // Do not call directly, use setSupportingStructures() method.
    void setOrderMap();

protected:

    uint64_t numberOfImages = 0;
    uint64_t imageHeight = 0;
    uint64_t imageWidth = 0;
    uint64_t numberOfPlanes = 1;
    std::vector<DataOrderIds> dataOrder = { DataOrderIds::NUMBER_OF_IMAGES, DataOrderIds::IMAGE_HEIGHT, DataOrderIds::IMAGE_WIDTH, DataOrderIds::NUMBER_OF_PLANES };
    std::vector<DataTypes> dataType = { DataTypes::FLOAT };
    char *_data = nullptr;

    uint64_t dataSize = 0;                              // size of the data according to given dimensions and data types
    uint64_t memorySize = 0;                            // size of the used memory
    uint64_t maxMemorySize = DEFAULT_MEMORY_SIZE;       // the maximal size of usable memory

    std::vector<uint64_t> offsets;                      // offsets of the outermost entities
    std::vector<uint64_t> entityTypeSizes;              // sizes of data types of the outermost entities
    std::vector<uint64_t> dimensions;                   // dimensions of the data according to dataOrder
    std::map<DataOrderIds, uint64_t> orderMap;          // mapping of data order to the current indices
    std::vector<uint64_t> subSizes;                     // sizes of sub-blocks of data according to dataOrder
};

#endif // _BIG_CORE_BASE_H_
