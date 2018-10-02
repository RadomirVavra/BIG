#include <algorithm>
#include <fstream>
#include <cstring>

#include "../include/big_core_base.h"
#include "../include/big_core_input.h"

BigCoreInput::BigCoreInput(const std::string& filename)
{
    openFile(filename);
}

BigCoreInput::~BigCoreInput()
{
    if (file.is_open()) file.close();
}

void BigCoreInput::openFile(const std::string& filename)
{
    // open file
    file.open(filename, std::ios_base::binary);
    if (!file) throw "Unable to open file for reading!";

    // check header
    char buffer[CHUNK_LENGTH];
    file.read(buffer, CHUNK_LENGTH);
    for (size_t i = 0; i != CHUNK_LENGTH; ++i) {
        if (buffer[i] != MAGIC[i]) throw "File format is not BIG format!";
    }

    // clear current content
    clear();
    dataPosition = 0;

    // load meta-data
    while (file) {
        CoreChunkIds id;
        uint64_t length;
        if (!readChunk(file, id, length) || !readData(file, id, length)) {
            throw "Error while reading file!";
        }
    }

    // check loaded values
    if (numberOfImages == 0) throw "Number of images in the file is zero!";
    if (imageHeight == 0) throw "Height of images in the file is zero!";
    if (imageWidth == 0) throw "Width of images in the file is zero!";
    if (dataPosition == 0) throw "Data are missing in the file!";

    // prepare supporting structures
    setSupportingStructures();
}

void BigCoreInput::readFile(const std::string& filename)
{
    openFile(filename);
    loadToMemory();
}

void BigCoreInput::readFile(const std::string& filename, uint64_t bytes)
{
    setMaxMemorySize(bytes);
    readFile(filename);
}

void BigCoreInput::loadToMemory()
{
    if (dataSize <= maxMemorySize)
    {
        // allocate new memory and load data to it
        memorySize = dataSize;
        _data = new char[memorySize];
        file.seekg(dataPosition);
        file.read(_data, dataSize);
    }
}

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

    if (isInMemory()) return operator()(imageNum, row, col, plane)

        // todo: load data from file and return them
        throw "Data not in memory!";
}

template<typename T>
const T* BigCoreInput::at(uint64_t index) const
{
    if (index >= dimensions[0])
        throw "Index out of bounds!";

    if (isInMemory()) return operator[](index);

    // todo: load data from file and return them
    throw "Data not in memory!";
}

template<typename T>
void BigCoreInput::getImage(T* data, uint64_t imageNum)
{
    if (imageNum >= numberOfImages)
        throw "Image number out of bound!";

    uint64_t index = 0;
    for (uint64_t row = 0; row != imageHeight; ++row) {
        for (uint64_t col = 0; col != imageWidth; ++col) {
            for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                data[index++] = operator()(imageNum, row, col, plane);
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

    uint64_t index = 0;
    for (uint64_t row = 0; row != imageHeight; ++row) {
        for (uint64_t col = 0; col != imageWidth; ++col) {
            for (uint64_t plane = 0; plane != numberOfPlanes; ++plane) {
                data[index++] = operator()(imageNum, row, col, plane);
            }
        }
    }
}

std::ifstream& BigCoreInput::readChunk(std::ifstream& file, CoreChunkIds& id, uint64_t& length)
{
    file.read(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
    file.read(reinterpret_cast<char*>(&length), CHUNK_LENGTH);
    return file;
}

std::ifstream& BigCoreInput::readData(std::ifstream &file, CoreChunkIds id, uint64_t length)
{
    switch (id) {
    case CoreChunkIds::NUMBER_OF_IMAGES:
        file.read(reinterpret_cast<char*>(&numberOfImages), sizeof(numberOfImages));
        file.seekg(length - sizeof(numberOfImages), std::ios_base::cur);
        break;
    case CoreChunkIds::IMAGE_HEIGHT:
        file.read(reinterpret_cast<char*>(&imageHeight), sizeof(imageHeight));
        file.seekg(length - sizeof(imageHeight), std::ios_base::cur);
        break;
    case CoreChunkIds::IMAGE_WIDTH:
        file.read(reinterpret_cast<char*>(&imageWidth), sizeof(imageWidth));
        file.seekg(length - sizeof(imageWidth), std::ios_base::cur);
        break;
    case CoreChunkIds::NUMBER_OF_PLANES:
        file.read(reinterpret_cast<char*>(&numberOfPlanes), sizeof(numberOfPlanes));
        file.seekg(length - sizeof(numberOfPlanes), std::ios_base::cur);
        break;
    case CoreChunkIds::DATA_ORDER:
    {
        dataOrder.clear();
        uint64_t n = length / sizeof(DataOrderIds);
        for (uint64_t i = 0; i != n; ++i) {
            DataOrderIds order;
            file.read(reinterpret_cast<char*>(&order), sizeof(DataOrderIds));
            dataOrder.push_back(order);
        }
        file.seekg(length - n * sizeof(DataOrderIds), std::ios_base::cur);
        break;
    }
    case CoreChunkIds::DATA_TYPE:
    {
        dataType.clear();
        uint64_t n = length / sizeof(DataTypes);
        for (uint64_t i = 0; i != n; ++i) {
            DataTypes type;
            file.read(reinterpret_cast<char*>(&type), sizeof(DataTypes));
            dataType.push_back(type);
        }
        file.seekg(length - n * sizeof(DataTypes), std::ios_base::cur);
        break;
    }
    case CoreChunkIds::DATA:
        dataPosition = file.tellg();
        break;
    default:
        file.seekg(length, std::ios_base::cur);
    }
    return file;
}
