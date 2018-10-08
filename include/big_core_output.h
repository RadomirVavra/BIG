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
    // If fileName is provided, dimensions of the container, data order or data type(s) cannot be changed any more, except whole entities can be inserted or removed.
    BigCoreOutput(uint64_t numberOfImages, uint64_t imageHeight, uint64_t imageWidth, uint64_t numberOfPlanes = 1, const std::vector<DataOrderIds>& dataOrder = defaultDataOrder, const std::vector<DataTypes>& dataType = defaultDataType, const std::string& fileName = "");

    // Copy constructor forbidden.
    BigCoreOutput(const BigCoreOutput &) = delete;

    // Move constructor.
    BigCoreOutput(BigCoreOutput &&other) = default;

    // Destructor. 
    ~BigCoreOutput();

    // Copy-assignment operator forbidden.
    BigCoreOutput &operator=(const BigCoreOutput &) = delete;

    // Move-assignment operator.
    BigCoreOutput &operator=(BigCoreOutput &&rhs) = default;

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

    // Opens a file, but does not write any data, except magic number.
    // Dimensions of the container, data order or data type(s) cannot be changed any more, except whole entities can be inserted or removed.
    void openFile(const std::string& fileName);

    // Writes all meta-data and un-saved data from cache and closes attached file.
    void closeFile();

    // Writes all data into the attached file. Size of the array must be at least size().
    // Does not add data to cache.
    void addData(std::shared_ptr<const char> data);

    // Writes all un-saved data from cache into the attached file.
    void flushCache();

    // Adds element in the given image at given row, column and (color) plane. Checks bounds.
    template<typename T>
    void addElement(const T& value, uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0);

    // Adds element in the given image at given row, column and (color) plane. Does not check bounds.
    template<typename T>
    void addElementFast(const T& value, uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0);

    // Adds the entity to the container. Checks bounds.
    template<typename T>
    void addEntity(std::shared_ptr<const T> data, uint64_t index);

    // Adds the entity to the container. Does not check bounds.
    template<typename T>
    void addEntityFast(std::shared_ptr<const T> data, uint64_t index);

    // Adds the image specified by its number.
    // Data must be of size height x width x #planes x sizeof(T).
    template<typename T>
    void addImage(std::shared_ptr<const T> data, uint64_t imageNum);

    // Adds the image specified by its number.
    // Data must be of size height x width x #planes x sizeof(T).
    template<typename T>
    void addImage(const std::vector<T>& data, uint64_t imageNum);

    // TODO
    // ------------------------------------------------------------------
    //// Adds new entity at the end.
    //// Data must be of size returned by getEntitySize(index) method.
    //template<typename T>
    //void push_back(std::shared_ptr<T> data);

    //// Removes the last entity.
    //void pop_back();

    //// Adds new entity before current entity specified by its index.
    //// If index is greater than or equal to number of entities, adds entity at the end.
    //// Data must be of size returned by getEntitySize(index) method.
    //template<typename T>
    //void insert(std::shared_ptr<T> data, uint64_t index);

    //// Removes entity specified by its index.
    //void erase(uint64_t index);

    //// Swaps entities specified by their numbers.
    //void swap(uint64_t index1, uint64_t index2);
    // ------------------------------------------------------------------


protected:

    // Write chunk's id, length and data.
    bool writeChunk(CoreChunkIds id);

    //// Write the data itself.
    //bool writeData();

protected:

    std::ofstream file;             // attached file
    bool locked = false;            // Indicates whether data array is locked. Once locked, dimensions, data order or data types cannot be modified.
};



template<typename T>
void BigCoreOutput::addElement(const T& value, uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane)
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bounds!";

    if (row >= imageHeight)
        throw "Row out of bounds!";

    if (col >= imageWidth)
        throw "Column out of bounds!";

    if (plane >= numberOfPlanes)
        throw "Plane number out of bounds!";

    std::array<uint64_t, 4> indices;
    indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
    indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
    indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
    indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;

    uint64_t index = indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3];

    cache.addElement(value, indices[0], index, file);
}

template<typename T>
void BigCoreOutput::addElementFast(const T& value, uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane)
{
    std::array<uint64_t, 4> indices;
    indices[orderMap[DataOrderIds::NUMBER_OF_IMAGES]] = imageNum;
    indices[orderMap[DataOrderIds::IMAGE_HEIGHT]] = row;
    indices[orderMap[DataOrderIds::IMAGE_WIDTH]] = col;
    indices[orderMap[DataOrderIds::NUMBER_OF_PLANES]] = plane;

    uint64_t index = indices[1] * subSize[1] + indices[2] * subSize[2] + indices[3];

    cache.addElement(value, indices[0], index, file);
}

template<typename T>
void BigCoreOutput::addEntity(std::shared_ptr<const T> data, uint64_t index)
{
    if (index >= dimensions[0])
        throw "Index out of bounds!";

    cache.addEntity(std::dynamic_pointer_cast<char>(data), index, file);
}

template<typename T>
void BigCoreOutput::addEntityFast(std::shared_ptr<const T> data, uint64_t index)
{
    cache.addEntity(std::dynamic_pointer_cast<char>(data), index, file);
}

template<typename T>
void BigCoreOutput::addImage(std::shared_ptr<const T> data, uint64_t imageNum)
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
                cache.addElement<T>(data[index++], indices[0], i, file);
            }
        }
    }
}

template<typename T>
void BigCoreOutput::addImage(const std::vector<T>& data, uint64_t imageNum)
{
    if (data.size() < subSizes[0])
        thow "Not enough data provided!";

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
                cache.addElement<T>(data[index++], indices[0], i, file);
            }
        }
    }
}

// TODO
// ------------------------------------------------------------------
//// Adds new entity at the end.
//// Data must be of size returned by getEntitySize(index) method.
//template<typename T>
//void BigCoreOutput::push_back(std::shared_ptr<T> data)
//{
//    // todo
//}
//
//// Adds new entity before current entity specified by its index.
//// If index is greater than or equal to number of entities, adds entity at the end.
//// Data must be of size returned by getEntitySize(index) method.
//template<typename T>
//void BigCoreOutput::insert(std::shared_ptr<T> data, uint64_t index)
//{
//    // todo
//}
// ---------------------------------------------------------------------


#endif // _BIG_CORE_OUTPUT_H_ 