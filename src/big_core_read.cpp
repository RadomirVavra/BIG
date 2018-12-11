#include "../include/big_core_read.hpp"

namespace big
{
    BigCoreRead::BigCoreRead(const std::string& fileName, bool loadToCache, uint64_t cacheSizeBytes)
        : BigCoreReadInfo(fileName)
    {
        cache.initialize(entitySizes, dataPositions);
        cache.setSize(cacheSizeBytes);
        if (loadToCache) cache.load(file);
    }

    template<typename T>
    T BigCoreRead::operator()(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane)
    {
        std::array<uint64_t, 4> indices;
        indices[orderMap[ChunkIds::NUMBER_OF_IMAGES]] = imageNum;
        indices[orderMap[ChunkIds::IMAGE_HEIGHT]] = row;
        indices[orderMap[ChunkIds::IMAGE_WIDTH]] = col;
        indices[orderMap[ChunkIds::NUMBER_OF_PLANES]] = plane;

        uint64_t index = indices[1] * subSizes[1] + indices[2] * subSizes[2] + indices[3];

        return cache.getElement<T>(indices[0], index, file);
    }

    template uint8_t BigCoreRead::operator()(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane);

    template<typename T>
    T BigCoreRead::at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane)
    {
        if (imageNum >= numberOfImages)
            throw "Image number out of bounds!";

        if (row >= imageHeight)
            throw "Row out of bounds!";

        if (col >= imageWidth)
            throw "Column out of bounds!";

        if (plane >= numberOfPlanes)
            throw "Plane number out of bounds!";

        return operator() < T > (imageNum, row, col, plane);
    }

    std::shared_ptr<const char> BigCoreRead::operator[](uint64_t index)
    {
        //return std::dynamic_pointer_cast<const char>(cache.getEntity(index, file));
        return std::shared_ptr<const char>();
    }


    template<typename T>
    std::shared_ptr<T> BigCoreRead::getImage(uint64_t imageNum)
    {
        if (imageNum >= numberOfImages)
            throw "Image number out of bound!";

        std::array<uint64_t, 4> indices;
        indices[orderMap[ChunkIds::NUMBER_OF_IMAGES]] = imageNum;
        uint64_t index = 0;
        for (uint64_t row = 0; row != imageHeight; ++row) {
            indices[orderMap[ChunkIds::IMAGE_HEIGHT]] = row;
            for (uint64_t col = 0; col != imageWidth; ++col) {
                indices[orderMap[ChunkIds::IMAGE_WIDTH]] = col;
                for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                    indices[orderMap[ChunkIds::NUMBER_OF_PLANES]] = plane;
                    uint64_t i = indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3];
                    data[index++] = cache.getElement<T>(indices[0], i, file);
                }
            }
        }
    }


}
