#pragma once
#ifndef _BIG_CORE_BASE_H_
#define _BIG_CORE_BASE_H_

#include "big_consts.hpp"

#include <cstdint>
#include <string>
#include <map>
#include <vector>

namespace big
{
    class BigCoreBase
    {
    protected:

        // Default constructor.
        BigCoreBase() = default;

        // Copy constructor forbidden.
        BigCoreBase(const BigCoreBase &) = delete;

        // Move constructor.
        BigCoreBase(BigCoreBase &&other) = default;

        // Copy-assignment operator forbidden.
        BigCoreBase &operator=(const BigCoreBase &) = delete;

        // Move-assignment operator.
        BigCoreBase &operator=(BigCoreBase &&rhs) = default;

        // Destructor.
        ~BigCoreBase() = default;

    public:

        // Returns number of contained images.
        uint64_t getNumberOfImages() { return numberOfImages; }

        // Returns height of images.
        uint64_t getImageHeight() { return imageHeight; }

        // Returns width of images.
        uint64_t getImageWidth() { return imageWidth; }

        // Returns number of (color) planes.
        uint64_t getNumberOfPlanes() { return numberOfPlanes; }

        // Returns order in which the data are serialized into memory.
        const std::vector<ChunkIds>& getDataOrder() { return dataOrder; }

        // Returns type(s) of data.
        const std::vector<DataTypes>& getDataType() { return dataType; }

        // Returns i_th dimension of the container.
        uint64_t getDimension(uint64_t i);

        // Returns size of the outermost entity specified by its number.
        uint64_t getEntitySize(uint64_t index);

        // Returns number of elements of an outermost entity.
        uint64_t getEntityBaseSize();

        // Returns data type of the outermost entity specified by its number.
        DataTypes getEntityDataType(uint64_t index);

        // Returns size of data type of the outermost entity specified by its number.
        uint64_t getEntityTypeSize(uint64_t index);

        // Returns size of the given data type.
        static uint64_t getDataTypeSize(DataTypes dataType);

        // Returns number of outermost entities.
        uint64_t getNumberOfEntities();

        // Returns total size of the data.
        uint64_t getSize() { return dataSize; }

    protected:

        // Initializes supporting data structures.
        void initSupportingStructures();

    private:
        // Do not call directly, use initSupportingStructures() method.
        void initDimensions();

        // Do not call directly, use initSupportingStructures() method.
        void initEntityTypeSizes();

        // Do not call directly, use initSupportingStructures() method.
        void initSubSizes();

        // Do not call directly, use initSupportingStructures() method.
        void initEntitySizes();

        // Do not call directly, use initSupportingStructures() method.
        void initDataSize();

        // Do not call directly, use initSupportingStructures() method.
        void initOffsets();

        // Do not call directly, use initSupportingStructures() method.
        void initOrderMap();

    protected:

        uint64_t numberOfImages = 0;
        uint64_t imageHeight = 0;
        uint64_t imageWidth = 0;
        uint64_t numberOfPlanes = 1;
        std::vector<ChunkIds> dataOrder = defaultDataOrder;
        std::vector<DataTypes> dataType = defaultDataType;

        std::vector<uint64_t> offsets;                      // offsets of the outermost entities
        std::vector<uint64_t> entityTypeSizes;              // sizes of data types of the outermost entities
        std::vector<uint64_t> entitySizes;                  // sizes of the outermost entities
        std::vector<uint64_t> dimensions;                   // dimensions of the data according to dataOrder
        std::map<ChunkIds, uint64_t> orderMap;              // mapping of data order to the current indices
        std::vector<uint64_t> subSizes;                     // sizes of sub-blocks of data according to dataOrder
        uint64_t dataSize = 0;                              // size of the data according to given dimensions and data types
        std::vector<uint64_t> dataPositions;                // positions of the data entities in a file
    };
}

#endif // _BIG_CORE_BASE_H_
