#include "../include/big_core_input.h"

BigCoreInput::BigCoreInput(const std::string& fileName)
{
    openFile(fileName);
}

BigCoreInput::~BigCoreInput()
{
    if (file.is_open()) file.close();
}

void BigCoreInput::openFile(const std::string& fileName)
{
    // open file
    file.open(fileName, std::ios_base::binary);
    if (!file) throw "Unable to open file for reading!";

    // check header
    char buffer[CHUNK_LENGTH];
    file.read(buffer, CHUNK_LENGTH);
    for (size_t i = 0; i != CHUNK_LENGTH; ++i) {
        if (buffer[i] != MAGIC[i]) throw "File format is not BIG format!";
    }

    // clear current content
    clear();

    // load meta-data and remember position of the data
    while (file) {
        CoreChunkIds id;
        uint64_t length;
        if (!readChunk(id, length) || !readData(id, length)) {
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

void BigCoreInput::readFile(const std::string& fileName)
{
    openFile(fileName);
    loadToMemory();
}

void BigCoreInput::readFile(const std::string& fileName, uint64_t bytes)
{
    setMaxMemorySize(bytes);
    readFile(fileName);
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

bool BigCoreInput::readChunk(CoreChunkIds& id, uint64_t& length)
{
    file.read(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
    file.read(reinterpret_cast<char*>(&length), CHUNK_LENGTH);
    return !file.fail();
}

bool BigCoreInput::readData(CoreChunkIds id, uint64_t length)
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
    return !file.fail();
}
