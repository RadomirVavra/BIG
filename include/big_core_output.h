#pragma once
#ifndef _BIG_CORE_OUTPUT_H_
#define _BIG_CORE_OUTPUT_H_

#include "big_core_base.h"

#include <fstream>
#include <string>

class BigCoreOutput : public BigCoreBase
{
public:

    // Empty constructor. Constructs an empty container, with no elements.
    BigCoreOutput() {}

    // Reserve constructor. Constructs an empty container, sets dimensions of the container, data order and data type(s).
    // If fileName is not empty, opens a file and writes meta-data, but does not write the data itself yet.
    // Calls reserve() method.
    BigCoreOutput(uint64_t numberOfImages, uint64_t imageHeight, uint64_t imageWidth, uint64_t numberOfPlanes = 1, const std::vector<DataOrderIds>& dataOrder = defaultDataOrder, const std::vector<DataTypes>& dataType = defaultDataType, const std::string& fileName = "");

    // Destructor. 
    ~BigCoreOutput();

    // Opens a file and writes meta-data, but does not write the data itself yet.
    void openFile(const std::string& fileName);

    // Writes meta-data and the data itself to a file. If fileName is empty, uses file specified earlier through constructor or openFile method.
    // Keeps file open for additional writes when new data are provided.
    void writeFile(const std::string& fileName = "");

    // Writes unsaved data and closes attached file.
    void closeFile();

    // Sets dimensions of the container, data order and data type(s).
    void setParameters(uint64_t numberOfImages, uint64_t imageHeight, uint64_t imageWidth, uint64_t numberOfPlanes = 1, const std::vector<DataOrderIds>& dataOrder = defaultDataOrder, const std::vector<DataTypes>& dataType = defaultDataType);

    // Sets number of images.
    void setNumberOfImages(uint64_t numberOfImages) { if (!locked) this->numberOfImages = numberOfImages; }

    // Sets height of images.
    void setImageHeight(uint64_t imageHeight) { if (!locked) this->imageHeight = imageHeight; }

    // Sets width of images.
    void setImageWidth(uint64_t imageWidth) { if (!locked) this->imageWidth = imageWidth; }

    // Sets number of (color) planes.
    void setNumberOfPlanes(uint64_t numberOfPlanes = 1) { if (!locked) this->numberOfPlanes = numberOfPlanes; }

    // Sets order in which the data are serialized into memory
    void setDataOrder(const std::vector<DataOrderIds>& dataOrder = defaultDataOrder) { if (!locked) this->dataOrder = dataOrder; }

    // Sets type(s) of data.
    void setDataType(const std::vector<DataTypes>& dataType = defaultDataType) { if (!locked) this->dataType = dataType; }

    // If possible, reserves required space in a memory to store provided data.
    // If it is not possible and if fileName has been provided, reserves required space in a file.
    // Dimensions of the container, data order or data type(s) cannot be changed any more.
    void reserve();

    // Sets maximal possible size of memory that can be used and if possible, reserves required space in the memory to store provided data.
    // If it is not possible and if fileName has been provided, reserves required space in a file.
    // Dimensions of the container, data order or data type(s) cannot be changed any more.
    void reserve(uint64_t bytes);

    // Returns a direct pointer to the memory array used internally by the container to store its owned elements.
    char* data() { return _data; }

    // Access element in a given image at given row, column and (color) plane. Does not check bounds.
    template<typename T>
    T & operator() (uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0);

    // Access the outermost entity of the container. Does not check bounds.
    template<typename T>
    T * operator[] (uint64_t index);

    // Access element in a given image at given row, column and (color) plane. Checks bounds.
    template<typename T>
    T & at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0);

    // Access the outermost entity of the container. Checks bounds.
    template<typename T>
    T * at(uint64_t index);

    // Adds an image specified by its number.
    // Data must be of size height x width x #planes x sizeof(T).
    template<typename T>
    void addImage(const T *data, uint64_t imageNum);

    // Adds an image specified by its number.
    // Data must be of size height x width x #planes x sizeof(T).
    template<typename T>
    void addImage(const std::vector<T>& data, uint64_t imageNum);

protected:

    // Write chunk's id, length and data.
    bool writeChunk(CoreChunkIds id);

    // Write the data itself.
    bool writeData();

protected:

    std::ofstream file;             // attached file
    bool locked = false;            // Indicates whether data array is locked. Once locked, dimensions, data order or data types cannot be modified.
    bool changed = false;           // Indicates whether data has been changed after last write to file.
};


template<typename T>
T& BigCoreOutput::operator()(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane)
{
    changed = true;
    std::array<uint64_t, 4> indices;
    indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
    indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
    indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
    indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;

    uint64_t index = offsets[indices[0]] + (indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3]) * entityTypeSizes[indices[0]];
    return static_cast<T&>(_data[index]);
}

template<typename T>
T* BigCoreOutput::operator[](uint64_t index)
{
    changed = true;
    return static_cast<T*>(_data + offsets[index]);
}

template<typename T>
T& BigCoreOutput::at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane)
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bounds!";

    if (row >= imageHeight)
        throw "Row out of bounds!";

    if (col >= imageWidth)
        throw "Column out of bounds!";

    if (plane >= numberOfPlanes)
        throw "Plane number out of bounds!";

    if (isInMemory()) {
        changed = true;
        return operator()(imageNum, row, col, plane);
    }

    // todo: load data from file and return them
    throw "Data not in memory!";
}

template<typename T>
T* BigCoreOutput::at(uint64_t index)
{
    if (index >= dimensions[0])
        throw "Index out of bounds!";

    if (isInMemory()) {
        changed = true;
        return operator[](index);
    }

    // todo: load data from file and return them
    throw "Data not in memory!";
}

template<typename T>
void BigCoreOutput::addImage(const T* data, uint64_t imageNum)
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bound!";

    uint64_t index = 0;
    for (uint64_t row = 0; row != imageHeight; ++row) {
        for (uint64_t col = 0; col != imageWidth; ++col) {
            for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                operator()(imageNum, row, col, plane) = data[index++];
            }
        }
    }
}

template<typename T>
void BigCoreOutput::addImage(const std::vector<T>& data, uint64_t imageNum)
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bound!";

    data.resize(imageHeight * imageWidth * numberOfPlanes);

    uint64_t index = 0;
    for (uint64_t row = 0; row != imageHeight; ++row) {
        for (uint64_t col = 0; col != imageWidth; ++col) {
            for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                operator()(imageNum, row, col, plane) = data[index++];
            }
        }
    }
}


#endif // _BIG_CORE_OUTPUT_H_ 