#pragma once
#ifndef _BIG_CORE_INPUT_H_
#define _BIG_CORE_INPUT_H_

#include "big_core_base.h"

#include <fstream>
#include <array>
#include <memory>

class BigCoreInput : public BigCoreBase
{
public:

    // Empty constructor. Constructs an empty container, with no elements.
    BigCoreInput() {}

    // Input constructor. Opens file, reads parameters of the container, but does not read data.
    BigCoreInput(const std::string& fileName);

    // Copy constructor forbidden.
    BigCoreInput(const BigCoreInput &) = delete;

    // Move constructor.
    BigCoreInput(BigCoreInput &&other) = default;

    // Destructor.
    ~BigCoreInput();

    // Copy-assignment operator forbidden.
    BigCoreInput &operator=(const BigCoreInput &) = delete;

    // Move-assignment operator.
    BigCoreInput &operator=(BigCoreInput &&rhs) = default;

    // Opens file, reads parameters of the container, but does not read data.
    void openFile(const std::string& fileName);

    // Closes file. Data in cache are still accessible until new file is opened.
    void closeFile();

    // Loads all data into the provided array. Size of the array must be at least size().
    // Does not load data to cache.
    void getData(std::shared_ptr<char> data);

    // Loads all data to cache, if possible. Does nothing if data are bigger than cache size. 
    void loadToCache();

    // Sets maximal possible size of cache that can be used and loads all data to cache, if possible.
    // Does nothing if data are bigger than cache size. 
    void loadToCache(uint64_t bytes);

    // Returns element in the given image at given row, column and (color) plane. Does not check bounds.
    template<typename T>
    T operator() (uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0);

    // Returns element in the given image at given row, column and (color) plane. Checks bounds.
    template<typename T>
    T at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0);

    // Returns pointer to the outermost entity of the container specified by its number. Does not check bounds.
    template<typename T>
    std::shared_ptr<const T> operator[] (uint64_t index);

    // Returns pointer to the outermost entity of the container specified by its number. Checks bounds.
    template<typename T>
    std::shared_ptr<const T> at(uint64_t index);

    // Returns the image specified by its number.
    // Data must be of size at least height x width x #planes x sizeof(T).
    template<typename T>
    void getImage(std::shared_ptr<T> data, uint64_t imageNum);

    // Returns the image specified by its number.
    template<typename T>
    void getImage(std::vector<T>& data, uint64_t imageNum);

protected:

    // Reads chunk's id and its length.
    bool readChunk(CoreChunkIds& id, uint64_t& length);

    // Reads chunk's data, e.g., numberOfImages, imageHeight, etc. Does not read the data itself, just remembers its position.
    bool readData(CoreChunkIds id, uint64_t length);

protected:

    std::ifstream file;     // attached file
};


template<typename T>
T BigCoreInput::operator()(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane)
{
    std::array<uint64_t, 4> indices;
    indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
    indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
    indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
    indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;

    uint64_t index = indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3];

    return cache.getElement<T>(indices[0], index, file);
}

template<typename T>
T BigCoreInput::at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane)
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bounds!";

    if (row >= imageHeight)
        throw "Row out of bounds!";

    if (col >= imageWidth)
        throw "Column out of bounds!";

    if (plane >= numberOfPlanes)
        throw "Plane number out of bounds!";

    return operator()<T>(imageNum, row, col, plane);
}

template<typename T>
std::shared_ptr<const T> BigCoreInput::operator[](uint64_t index)
{
    return std::dynamic_pointer_cast<const T>(cache.getEntity(index, file));
}

template<typename T>
std::shared_ptr<const T> BigCoreInput::at(uint64_t index)
{
    if (index >= dimensions[0])
        throw "Index out of bounds!";

    return std::dynamic_pointer_cast<const T>(cache.getEntity(index, file));
}

template<typename T>
void BigCoreInput::getImage(std::shared_ptr<T> data, uint64_t imageNum)
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bound!";

    std::array<uint64_t, 4> indices;
    indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
    uint64_t index = 0;
    for (uint64_t row = 0; row != imageHeight; ++row) {
        indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
        for (uint64_t col = 0; col != imageWidth; ++col) {
            indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
            for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;
                uint64_t i = indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3];
                data[index++] = cache.getElement<T>(indices[0], i, file);
            }
        }
    }
}

template<typename T>
void BigCoreInput::getImage(std::vector<T>& data, uint64_t imageNum)
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bound!";

    data.resize(imageHeight * imageWidth * numberOfPlanes);
    std::array<uint64_t, 4> indices;
    indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
    uint64_t index = 0;
    for (uint64_t row = 0; row != imageHeight; ++row) {
        indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
        for (uint64_t col = 0; col != imageWidth; ++col) {
            indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
            for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;
                uint64_t i = indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3];
                data[index++] = cache.getElement<T>(indices[0], i, file);
            }
        }
    }
}

#endif // _BIG_CORE_INPUT_H_
