#pragma once
#ifndef _BIG_CORE_CONSTS_H_
#define _BIG_CORE_CONSTS_H_

#include <cstdint>
#include <string>
//#include <map>
//#include <array>
#include <vector>

// default size of usable memory
const uint64_t DEFAULT_CACHE_SIZE = 1024ull * 1024ull * 1024ull;

// length of chunk
const uint64_t CHUNK_LENGTH = 8;

// big magic number
const std::string MAGIC = { 0x42, 0x49, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00 };

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

// core chunk ids in vector
const std::vector<CoreChunkIds> CoreChunkIds_ = { CoreChunkIds::NUMBER_OF_IMAGES, CoreChunkIds::IMAGE_HEIGHT, CoreChunkIds::IMAGE_WIDTH, CoreChunkIds::NUMBER_OF_PLANES, CoreChunkIds::DATA_ORDER, CoreChunkIds::DATA_TYPE, CoreChunkIds::DATA };

// data order ids
enum class DataOrderIds : uint64_t
{
    NUMBER_OF_IMAGES = 1,
    IMAGE_HEIGHT = 2,
    IMAGE_WIDTH = 3,
    NUMBER_OF_PLANES = 4,
};

// default order of data
const std::vector<DataOrderIds> defaultDataOrder = { DataOrderIds::NUMBER_OF_IMAGES, DataOrderIds::IMAGE_HEIGHT, DataOrderIds::IMAGE_WIDTH, DataOrderIds::NUMBER_OF_PLANES };

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

// default type of data
const std::vector<DataTypes> defaultDataType = { DataTypes::FLOAT };

// sizes of data types
const std::vector<uint64_t> typeSizes = { 0, 2, 4, 8, 1, 1, 2, 2, 4, 4, 8, 8, 1 };
const uint64_t maxTypeSize = 8;

#endif // _BIG_CORE_CONSTS_H_
