#include "../include/big_core_output.h"

BigCoreOutput::BigCoreOutput(uint64_t numberOfImages, uint64_t imageHeight, uint64_t imageWidth, uint64_t numberOfPlanes, const std::vector<DataOrderIds>& dataOrder, const std::vector<DataTypes>& dataType, const std::string& fileName)
{
    this->numberOfImages = numberOfImages;
    this->imageHeight = imageHeight;
    this->imageWidth = imageWidth;
    this->numberOfPlanes = numberOfPlanes;
    this->dataOrder = dataOrder;
    this->dataType = dataType;
    if (!fileName.empty()) openFile(fileName);
}

BigCoreOutput::~BigCoreOutput()
{
    closeFile();
    delete[] _data;
}

void BigCoreOutput::openFile(const std::string& fileName)
{
    // close current file
    closeFile();
    changed = true;

    // open new file
    file.open(fileName, std::ios_base::binary | std::ios_base::out);
    if (!file) throw "Unable to open file for writing!";

    // write magic number
    file.write(MAGIC.c_str(), CHUNK_LENGTH);

    // write meta-data, but do not write the data itself
    for (const auto& id : CoreChunkIdsArray) {
        if (!writeChunk(id)) throw "Error while writing file!";
    }

    // reserve space in a memory or the file
    reserve();
}

void BigCoreOutput::writeFile(const std::string& fileName)
{
    if (!file || !fileName.empty()) {
        // close current file
        if (file) closeFile();
        changed = true;

        // open new file
        file.open(fileName, std::ios_base::binary | std::ios_base::out);
        if (!file) throw "Unable to open file for writing!";

        // write magic number
        file.write(MAGIC.c_str(), CHUNK_LENGTH);

        // write meta-data, but do not write the data itself
        for (const auto& id : CoreChunkIdsArray) {
            if (!writeChunk(id)) throw "Error while writing file!";
        }
    }

    if (changed) writeData();

}

void BigCoreOutput::closeFile()
{
    if (file.is_open()) {
        if (changed) writeData();
        file.close();
    }
}

void BigCoreOutput::setParameters(uint64_t numberOfImages, uint64_t imageHeight, uint64_t imageWidth, uint64_t numberOfPlanes, const std::vector<DataOrderIds>& dataOrder, const std::vector<DataTypes>& dataType)
{
    if (locked) return;
    this->numberOfImages = numberOfImages;
    this->imageHeight = imageHeight;
    this->imageWidth = imageWidth;
    this->numberOfPlanes = numberOfPlanes;
    this->dataOrder = dataOrder;
    this->dataType = dataType;
}

void BigCoreOutput::reserve()
{
    if (locked) return;
    locked = true;
    setSupportingStructures();
    if (dataSize <= maxMemorySize) {
        memorySize = dataSize;
        _data = new char[memorySize];
    }
    else {
        writeData();
    }
}

void BigCoreOutput::reserve(uint64_t bytes)
{
    setMaxMemorySize(bytes);
    reserve();
}


bool BigCoreOutput::writeChunk(CoreChunkIds id)
{
    const uint64_t zero = 0;
    switch (id) {
    case CoreChunkIds::NUMBER_OF_IMAGES:
        file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
        file.write(reinterpret_cast<const char*>(&CHUNK_LENGTH), CHUNK_LENGTH);
        file.write(reinterpret_cast<const char*>(&numberOfImages), sizeof(numberOfImages));
        file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - sizeof(numberOfImages));
        break;
    case CoreChunkIds::IMAGE_HEIGHT:
        file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
        file.write(reinterpret_cast<const char*>(&CHUNK_LENGTH), CHUNK_LENGTH);
        file.write(reinterpret_cast<const char*>(&imageHeight), sizeof(imageHeight));
        file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - sizeof(imageHeight));
        break;
    case CoreChunkIds::IMAGE_WIDTH:
        file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
        file.write(reinterpret_cast<const char*>(&CHUNK_LENGTH), CHUNK_LENGTH);
        file.write(reinterpret_cast<const char*>(&imageWidth), sizeof(imageWidth));
        file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - sizeof(imageWidth));
        break;
    case CoreChunkIds::NUMBER_OF_PLANES:
        file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
        file.write(reinterpret_cast<const char*>(&CHUNK_LENGTH), CHUNK_LENGTH);
        file.write(reinterpret_cast<const char*>(&numberOfPlanes), sizeof(numberOfPlanes));
        file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - sizeof(numberOfPlanes));
        break;
    case CoreChunkIds::DATA_ORDER:
    {
        file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
        uint64_t length = static_cast<uint64_t>(std::ceil((dataOrder.size() * sizeof(DataOrderIds)) / static_cast<double>(CHUNK_LENGTH))) * CHUNK_LENGTH;
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        for (const auto& i : dataOrder) {
            file.write(reinterpret_cast<const char*>(&i), sizeof(DataOrderIds));
        }
        file.write(reinterpret_cast<const char*>(&zero), length - dataOrder.size() * sizeof(DataOrderIds));
        break;
    }
    case CoreChunkIds::DATA_TYPE:
    {
        file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
        uint64_t length = static_cast<uint64_t>(std::ceil((dataType.size() * sizeof(DataTypes)) / static_cast<double>(CHUNK_LENGTH))) * CHUNK_LENGTH;
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        for (const auto& i : dataType) {
            file.write(reinterpret_cast<const char*>(&i), sizeof(DataTypes));
        }
        file.write(reinterpret_cast<const char*>(&zero), length - dataType.size() * sizeof(DataTypes));
        break;
    }
    }
    return !file.fail();
}

bool BigCoreOutput::writeData()
{
    if (dataPosition == 0) {
        const auto id = CoreChunkIds::DATA;
        file.write(reinterpret_cast<const char*>(&id), CHUNK_LENGTH);
        const uint64_t n = static_cast<uint64_t>(std::ceil(dataSize / static_cast<double>(CHUNK_LENGTH)));
        const uint64_t length = n * CHUNK_LENGTH;
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        dataPosition = file.tellp();
        if (!isInMemory()) {
            const uint64_t zero = 0;
            for (uint64_t i = 0; i != n; ++i) {
                file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH);
            }
        }
    }
    else {
        file.seekp(dataPosition);
    }
    if (isInMemory()) {
        file.write(_data, dataSize);
        const uint64_t zero = 0;
        file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - dataSize % CHUNK_LENGTH);
    }
}
