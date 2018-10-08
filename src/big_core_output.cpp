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

void BigCoreOutput::openFile(const std::string& fileName)
{
    // close current file
    closeFile();

    // open file
    file.open(fileName, std::ios_base::binary | std::ios_base::out); // todo: we will need to re-read entity to cache, and to re-write entity index (when inserting new entity) and entity id to zero (for removing)
    if (!file) throw "Unable to open file for writing!";

    // write magic number
    file.write(MAGIC.c_str(), CHUNK_LENGTH);

    // lock dimensions of the container and prepare supporting structures
    locked = true;
    setSupportingStructures();
}

void BigCoreOutput::closeFile()
{
    if (file.is_open()) {

        // write meta-data, but do not write the data itself
        file.seekp(0, std::ios_base::end);
        for (const auto& id : CoreChunkIds_) {
            if (!writeChunk(id)) throw "Error while writing file!";
        }

        cache.flush(file);

        file.close();
    }
    cache.clear();
    locked = false;
}

void BigCoreOutput::addData(std::shared_ptr<const char> data)
{
    cache.clear();
    for (uint64_t i = 0; i != dimensions[0]; ++i) {
        if (dataPositions[i] == 0) {
            file.seekp(0, std::ios_base::end);
            const auto id = CoreChunkIds::DATA;
            file.write(reinterpret_cast<const char*>(&id), CHUNK_LENGTH);
            uint64_t length = entitySizes[i] + (entitySizes[i] % CHUNK_LENGTH > 0) ? CHUNK_LENGTH - entitySizes[i] % CHUNK_LENGTH : 0;
            file.write(reinterpret_cast<const char*>(&length), sizeof(length));
            file.write(reinterpret_cast<const char*>(&i), sizeof(i));
            dataPositions[i] = file.tellp();
        }
        else {
            file.seekp(dataPositions[i]);
        }
        file.write(data.get() + offsets[i], entitySizes[i]);
        const uint64_t zero = 0;
        file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - entitySizes[i] % CHUNK_LENGTH);
    }
}

void BigCoreOutput::flushCache()
{
    cache.flush(file);
}

// TODO
// ------------------------------------------------------------------
//// Removes the last entity.
//void BigCoreOutput::pop_back()
//{
//    // todo
//}
//
//// Removes entity specified by its index.
//void BigCoreOutput::erase(uint64_t index)
//{
//    // todo
//}
//
//// Swaps entities specified by their numbers.
//void BigCoreOutput::swap(uint64_t index1, uint64_t index2)
//{
//    // todo
//}
// ------------------------------------------------------------------



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
        uint64_t l = dataOrder.size() * sizeof(DataTypes);
        uint64_t length = l + (l % CHUNK_LENGTH > 0) ? CHUNK_LENGTH - l % CHUNK_LENGTH : 0;
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        for (const auto& i : dataOrder) {
            file.write(reinterpret_cast<const char*>(&i), sizeof(DataOrderIds));
        }
        file.write(reinterpret_cast<const char*>(&zero), length - l);
        break;
    }
    case CoreChunkIds::DATA_TYPE:
    {
        file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
        uint64_t l = dataType.size() * sizeof(DataTypes);
        uint64_t length = l + (l % CHUNK_LENGTH > 0) ? CHUNK_LENGTH - l % CHUNK_LENGTH : 0;
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        for (const auto& i : dataType) {
            file.write(reinterpret_cast<const char*>(&i), sizeof(DataTypes));
        }
        file.write(reinterpret_cast<const char*>(&zero), length - l);
        break;
    }
    }
    return !file.fail();
}
