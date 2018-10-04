#pragma once
#ifndef _BIG_CORE_INPUT_H_
#define _BIG_CORE_INPUT_H_

#include "big_core_base.h"

#include <fstream>
#include <array>

class BigCoreInput : public BigCoreBase
{
public:

    // Empty constructor. Constructs an empty container, with no elements.
    BigCoreInput() {}

    // Input constructor. Opens file, reads parameters of the container, but does not read data.
    BigCoreInput(const std::string& fileName);

    // Copy constructor forbidden.
    BigCoreInput(const BigCoreInput &) = delete;

    // Destructor.
    ~BigCoreInput();

    // Assignment operator forbidden.
    BigCoreInput &operator=(const BigCoreInput &) = delete;

    // Opens file, reads parameters of the container, but does not read data.
    void openFile(const std::string& fileName);

    // Opens file, reads parameters and data to memory, if possible.
    void readFile(const std::string& fileName);

    // Sets maximal possible size of memory that can be used and opens file, reads parameters and data to memory, if possible.
    void readFile(const std::string& fileName, uint64_t bytes);

    // Loads all data to memory, if possible. Does nothing if data are bigger than memory size. 
    void loadToMemory();

    // Returns a direct pointer to the memory array used internally by the container to store its owned elements.
    // Warning: all data must be in memory, use isInMemory() method to verify.
    const char* data() const { return _data; }

    // Access element in a given image at given row, column and (color) plane. Does not check bounds.
    // Warning: all data must be in memory, use isInMemory() method to verify.
    template<typename T>
    const T & operator() (uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0) const;

    // Access an outermost entity of the container specified by its number. Does not check bounds.
    // Warning: all data must be in memory, use isInMemory() method to verify.
    template<typename T>
    const T * operator[] (uint64_t index) const;

    // Access element in a given image at given row, column and (color) plane. Checks bounds.
    template<typename T>
    const T & at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0) const;

    // Access an outermost entity of the container specified by its number. Checks bounds.
    template<typename T>
    const T * at(uint64_t index) const;

    // Returns an image specified by its number.
    // Data must be of size at least height x width x #planes x sizeof(T).
    template<typename T>
    void getImage(T *data, uint64_t imageNum) const;

    // Returns an image specified by its number.
    template<typename T>
    void getImage(std::vector<T>& data, uint64_t imageNum) const;

    // Returns an outermost entity of the container specified by its number.
    // Data must be at least of size returned by getEntitySize(index) method.
    template<typename T>
    void getEntity(T *data, uint64_t index) const;

    // Returns an outermost entity of the container specified by its number.
    template<typename T>
    void getEntity(std::vector<T>& data, uint64_t index) const;

protected:

    // Reads chunk's id and its length.
    bool readChunk(CoreChunkIds& id, uint64_t& length);

    // Reads chunk's data, e.g., numberOfImages, imageHeight, etc. Does not read the data itself, just remembers its position.
    bool readData(CoreChunkIds id, uint64_t length);

protected:

    std::ifstream file;                 // attached file
};


template<typename T>
const T& BigCoreInput::operator()(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane) const
{
    std::array<uint64_t, 4> indices;
    indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
    indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
    indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
    indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;

    uint64_t index = offsets[indices[0]] + (indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3]) * entityTypeSizes[indices[0]];
    return static_cast<const T&>(_data[index]);
}

template<typename T>
const T* BigCoreInput::operator[](uint64_t index) const
{
    return static_cast<const T*>(_data + offsets[index]);
}

template<typename T>
const T& BigCoreInput::at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane) const
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bounds!";

    if (row >= imageHeight)
        throw "Row out of bounds!";

    if (col >= imageWidth)
        throw "Column out of bounds!";

    if (plane >= numberOfPlanes)
        throw "Plane number out of bounds!";

    if (isInMemory()) return operator()(imageNum, row, col, plane);

    std::array<uint64_t, 4> indices;
    indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
    indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
    indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
    indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;

    uint64_t index = (indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3]) * entityTypeSizes[indices[0]];
    char* d = cache.getEntity(indices[0], file);
    return static_cast<const T&>(d[index]);
}

template<typename T>
const T* BigCoreInput::at(uint64_t index) const
{
    if (index >= dimensions[0])
        throw "Index out of bounds!";

    if (isInMemory()) return operator[](index);

    return static_cast<const T*>(cache.getEntity(index, file));
}

template<typename T>
void BigCoreInput::getImage(T* data, uint64_t imageNum) const
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bound!";

    if (isInMemory()) {
        uint64_t index = 0;
        for (uint64_t row = 0; row != imageHeight; ++row) {
            for (uint64_t col = 0; col != imageWidth; ++col) {
                for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                    data[index++] = operator()(imageNum, row, col, plane);
                }
            }
        }
    }
    else {
        std::array<uint64_t, 4> indices;
        indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
        uint64_t index = 0;
        for (uint64_t row = 0; row != imageHeight; ++row) {
            indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
            for (uint64_t col = 0; col != imageWidth; ++col) {
                indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
                for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                    indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;
                    char* d = cache.getEntity(indices[0], file);
                    uint64_t i = (indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3]) * entityTypeSizes[indices[0]];
                    data[index++] = static_cast<const T&>(d[i]);
                }
            }
        }
    }
}

template<typename T>
void BigCoreInput::getImage(std::vector<T>& data, uint64_t imageNum) const
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bound!";

    data.resize(imageHeight * imageWidth * numberOfPlanes);

    switch (mode) {
    case 1:
        uint64_t index = 0;
        for (uint64_t row = 0; row != imageHeight; ++row) {
            for (uint64_t col = 0; col != imageWidth; ++col) {
                for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                    data[index++] = operator()(imageNum, row, col, plane);
                }
            }
        }
        break;
    case 2:
        std::array<uint64_t, 4> indices;
        indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
        uint64_t index = 0;
        for (uint64_t row = 0; row != imageHeight; ++row) {
            indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
            for (uint64_t col = 0; col != imageWidth; ++col) {
                indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
                for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                    indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;
                    char* d = cache.getEntity(indices[0], file);
                    uint64_t i = (indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3]) * entityTypeSizes[indices[0]];
                    data[index++] = static_cast<const T&>(d[i]);
                }
            }
        }
        break;
    case 3:
        // todo
    }
}


template<typename T>
void BigCoreInput::getEntity(T *data, uint64_t index) const
{
    if (index >= dimensions[0])
        throw "Index out of bounds!";

    switch (mode) {
    case 1:
        std::copy(_data + offsets[index], _data + offsets[index] + subSizes[0] * entityTypeSizes[index], data);
        break;
    case 2:
        char *d = cache.getEntity(index, file);
        std::copy(d, d + subSizes[0] * entityTypeSizes[index], data);
        break;
    case 3:
        // todo
        break;
    }
}

// Returns an outermost entity of the container specified by its number.
template<typename T>
void BigCoreInput::getEntity(std::vector<T>& data, uint64_t index) const
{
    if (index >= dimensions[0])
        throw "Index out of bounds!";

    switch (mode) {
    case 1:
        std::copy(_data + offsets[index], _data + offsets[index] + subSizes[0] * entityTypeSizes[index], data);
        break;
    case 2:
        char *d = cache.getEntity(index, file);
        std::copy(d, d + subSizes[0] * entityTypeSizes[index], data);
        break;
    case 3:
        // todo
        break;
    }
}

#endif // _BIG_CORE_INPUT_H_
