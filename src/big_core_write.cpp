#include "../include/big_core_write.hpp"

namespace big
{
    BigCoreWrite::BigCoreWrite(const std::string& fileName, uint64_t numberOfImages, uint64_t imageHeight, uint64_t imageWidth, uint64_t numberOfPlanes, const std::vector<ChunkIds>& dataOrder, const std::vector<DataTypes>& dataType)
    {
        this->numberOfImages = numberOfImages;
        this->imageHeight = imageHeight;
        this->imageWidth = imageWidth;
        this->numberOfPlanes = numberOfPlanes;
        this->dataOrder = dataOrder;
        this->dataType = dataType;

        // prepare supporting structures
        initSupportingStructures();

        // open file
        file.open(fileName, std::ios_base::binary | std::ios_base::out);
        if (!file) throw "Unable to open file for writing!";

        // write magic number
        file.write(MAGIC.c_str(), CHUNK_LENGTH);

        // write meta-data, but do not write the data itself
        for (const auto& id : CoreChunkIds) {
            if (!writeChunk(id)) throw "Error while writing file!";
        }


    }

    BigCoreWrite::~BigCoreWrite()
    {
        if (file.is_open()) file.close();
    }

    template<typename T>
    void BigCoreWrite::addEntity(std::shared_ptr<T> data, uint64_t index)
    {
        if (index >= dimensions[0])
            throw "Index out of bounds!";

        const auto id = ChunkIds::DATA;
        file.write(reinterpret_cast<const char*>(&id), CHUNK_LENGTH);
        uint64_t complement = entitySizes[index] % CHUNK_LENGTH > 0 ? CHUNK_LENGTH - entitySizes[index] % CHUNK_LENGTH : 0;
        uint64_t length = CHUNK_LENGTH + entitySizes[index] + complement;
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        file.write(reinterpret_cast<const char*>(&index), sizeof(index));
        file.write(reinterpret_cast<const char*>(data.get()), entitySizes[index]);
        const uint64_t zero = 0;
        file.write(reinterpret_cast<const char*>(&zero), complement);
    }

    template void BigCoreWrite::addEntity(std::shared_ptr<uint8_t> data, uint64_t index);
    template void BigCoreWrite::addEntity(std::shared_ptr<uint16_t> data, uint64_t index);



    bool BigCoreWrite::writeChunk(ChunkIds id)
    {
        const uint64_t zero = 0;
        switch (id) {
        case ChunkIds::NUMBER_OF_IMAGES:
            file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
            file.write(reinterpret_cast<const char*>(&CHUNK_LENGTH), CHUNK_LENGTH);
            file.write(reinterpret_cast<const char*>(&numberOfImages), sizeof(numberOfImages));
            file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - sizeof(numberOfImages));
            break;
        case ChunkIds::IMAGE_HEIGHT:
            file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
            file.write(reinterpret_cast<const char*>(&CHUNK_LENGTH), CHUNK_LENGTH);
            file.write(reinterpret_cast<const char*>(&imageHeight), sizeof(imageHeight));
            file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - sizeof(imageHeight));
            break;
        case ChunkIds::IMAGE_WIDTH:
            file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
            file.write(reinterpret_cast<const char*>(&CHUNK_LENGTH), CHUNK_LENGTH);
            file.write(reinterpret_cast<const char*>(&imageWidth), sizeof(imageWidth));
            file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - sizeof(imageWidth));
            break;
        case ChunkIds::NUMBER_OF_PLANES:
            file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
            file.write(reinterpret_cast<const char*>(&CHUNK_LENGTH), CHUNK_LENGTH);
            file.write(reinterpret_cast<const char*>(&numberOfPlanes), sizeof(numberOfPlanes));
            file.write(reinterpret_cast<const char*>(&zero), CHUNK_LENGTH - sizeof(numberOfPlanes));
            break;
        case ChunkIds::DATA_ORDER:
        {
            file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
            uint64_t l = dataOrder.size() * sizeof(DataTypes);
            uint64_t length = l + (l % CHUNK_LENGTH > 0 ? CHUNK_LENGTH - l % CHUNK_LENGTH : 0);
            file.write(reinterpret_cast<const char*>(&length), sizeof(length));
            for (const auto& i : dataOrder) {
                file.write(reinterpret_cast<const char*>(&i), sizeof(ChunkIds));
            }
            file.write(reinterpret_cast<const char*>(&zero), length - l);
            break;
        }
        case ChunkIds::DATA_TYPE:
        {
            file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
            uint64_t l = dataType.size() * sizeof(DataTypes);
            uint64_t length = l + (l % CHUNK_LENGTH > 0 ? CHUNK_LENGTH - l % CHUNK_LENGTH : 0);
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
}
