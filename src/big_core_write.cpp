#include "../include/big_core_write.hpp"

namespace big
{
    using half_float::half;
    using half_float::half_cast;
    BigCoreWrite::BigCoreWrite(const std::string& fileName,  uint64_t imageHeight, uint64_t imageWidth, uint64_t numberOfPlanes, const std::vector<ChunkIds>& dataOrder)
    {
        this->numberOfImages = 0;
        this->imageHeight = imageHeight;
        this->imageWidth = imageWidth;
        this->numberOfPlanes = numberOfPlanes;
        this->dataOrder = dataOrder;
        this->fileName = remove_extension(fileName);

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
        file.seekp(CHUNK_LENGTH);
        if (!writeChunk(ChunkIds::NUMBER_OF_IMAGES)) throw "Error while writing file!";
        file.seekp(0, std::ios_base::end);
        if (file.is_open()) file.close();
    }

    template<typename T>
    void BigCoreWrite::addEntity(std::shared_ptr<T> data, uint64_t index, DataTypes dataType)
    {
        numberOfImages++;
        /*file.seekp(CHUNK_LENGTH);
        if (!writeChunk(ChunkIds::NUMBER_OF_IMAGES)) throw "Error while writing file!";
        file.seekp(0, std::ios_base::end);*/

        if (index > numberOfImages)
            throw "Index out of bounds!";

        big::BigCoreBase::setEntitySizes(dataType); 

        const auto id = ChunkIds::DATA;
        file.write(reinterpret_cast<const char*>(&id), CHUNK_LENGTH);
        uint64_t complement = entitySizes[index] % CHUNK_LENGTH > 0 ? CHUNK_LENGTH - entitySizes[index] % CHUNK_LENGTH : 0;
        uint64_t length = 2*CHUNK_LENGTH + entitySizes[index] + complement;
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        file.write(reinterpret_cast<const char*>(&index), sizeof(index));
        file.write(reinterpret_cast<const char*>(&dataType), sizeof(DataTypes)); //write data type
        file.write(reinterpret_cast<const char*>(data.get()), entitySizes[index]);
        const uint64_t zero = 0;
        file.write(reinterpret_cast<const char*>(&zero), complement);

    }

    template void BigCoreWrite::addEntity(std::shared_ptr<uint8_t> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<uint16_t> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<uint32_t> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<uint64_t> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<int8_t> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<int16_t> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<int32_t> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<int64_t> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<half> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<float> data, uint64_t index, DataTypes dataType);
    template void BigCoreWrite::addEntity(std::shared_ptr<double> data, uint64_t index, DataTypes dataType);

    template<typename T>
    void BigCoreWrite::pushEntity(std::shared_ptr<T> data, DataTypes dataType)
    {
        uint64_t index = numberOfImages;
        numberOfImages++;
        

        if (index > numberOfImages)
            throw "Index out of bounds!";

        big::BigCoreBase::setEntitySizes(dataType);

        const auto id = ChunkIds::DATA;
        file.write(reinterpret_cast<const char*>(&id), CHUNK_LENGTH);
        uint64_t complement = entitySizes[index] % CHUNK_LENGTH > 0 ? CHUNK_LENGTH - entitySizes[index] % CHUNK_LENGTH : 0;
        uint64_t length = 2 * CHUNK_LENGTH + entitySizes[index] + complement;
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        file.write(reinterpret_cast<const char*>(&index), sizeof(index));
        file.write(reinterpret_cast<const char*>(&dataType), sizeof(DataTypes)); //write data type
        file.write(reinterpret_cast<const char*>(data.get()), entitySizes[index]);
        const uint64_t zero = 0;
        file.write(reinterpret_cast<const char*>(&zero), complement);

    }
    template void BigCoreWrite::pushEntity(std::shared_ptr<uint8_t> data,  DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<uint16_t> data,  DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<uint32_t> data,  DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<uint64_t> data, DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<int8_t> data,  DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<int16_t> data,  DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<int32_t> data, DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<int64_t> data,  DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<half> data,  DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<float> data,  DataTypes dataType);
    template void BigCoreWrite::pushEntity(std::shared_ptr<double> data,  DataTypes dataType);

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
        case ChunkIds::XML:
        {
          std::string xmlFileName = fileName + ".xml";
          uint64_t l = sizeof(xmlFileName);
          uint64_t length = l + (l % CHUNK_LENGTH > 0 ? CHUNK_LENGTH - l % CHUNK_LENGTH : 0);
          file.write(reinterpret_cast<char*>(&id), CHUNK_LENGTH);
          file.write(reinterpret_cast<const char*>(&length), sizeof(length));
          file.write(reinterpret_cast<const char*>(&xmlFileName[0]), sizeof(xmlFileName));
          file.write(reinterpret_cast<const char*>(&zero), length - l);
          break;
        }
      /*  case ChunkIds::DATA_TYPE:
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
        }*/
        }
        return !file.fail();
    }
    std::string BigCoreWrite::remove_extension(const std::string& fileName) {
      
      size_t lastdot = fileName.find_last_of(".");
      if (lastdot == std::string::npos) return fileName;
      return fileName.substr(0, lastdot);
      
    }
}
