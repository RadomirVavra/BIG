#include "../include/big_cache_read.hpp"

#include "../include/half.hpp"

namespace big
{
    using half_float::half;
    using namespace half_float::literal;

    template<> uint8_t BigCacheRead::convert(uint8_t value) { return value; }
    template<> uint8_t BigCacheRead::convert(uint16_t value) { return static_cast<uint8_t>(value >> 8); }
    template<> uint8_t BigCacheRead::convert(uint32_t value) { return static_cast<uint8_t>(value >> 24); }
    template<> uint8_t BigCacheRead::convert(uint64_t value) { return static_cast<uint8_t>(value >> 56); }
    template<> uint8_t BigCacheRead::convert(int8_t value) { return value <= 0 ? 0 : static_cast<uint8_t>((value << 1) + (value >> 6)); }
    template<> uint8_t BigCacheRead::convert(int16_t value) { return value <= 0 ? 0 : static_cast<uint8_t>(value >> 7); }
    template<> uint8_t BigCacheRead::convert(int32_t value) { return value <= 0 ? 0 : static_cast<uint8_t>(value >> 23); }
    template<> uint8_t BigCacheRead::convert(int64_t value) { return value <= 0 ? 0 : static_cast<uint8_t>(value >> 55); }
    template<> uint8_t BigCacheRead::convert(half value) { return value <= 0 ? 0 : value > 1.0_h ? 255 : static_cast<uint8_t>(value * 255.0_h + 0.5_h); }
    template<> uint8_t BigCacheRead::convert(float value) { return value <= 0 ? 0 : value > 1.0f ? 255 : static_cast<uint8_t>(value * 255.0f + 0.5f); }
    template<> uint8_t BigCacheRead::convert(double value) { return value <= 0 ? 0 : value > 1.0 ? 255 : static_cast<uint8_t>(value * 255.0 + 0.5); }

    template<> uint16_t BigCacheRead::convert(uint8_t value) { return (static_cast<uint16_t>(value) << 8) + value; }
    template<> uint16_t BigCacheRead::convert(uint16_t value) { return value; }
    template<> uint16_t BigCacheRead::convert(uint32_t value) { return static_cast<uint16_t>(value >> 16); }
    template<> uint16_t BigCacheRead::convert(uint64_t value) { return static_cast<uint16_t>(value >> 48); }
    template<> uint16_t BigCacheRead::convert(int8_t value) { return value <= 0 ? 0 : static_cast<uint16_t>((value << 9) + ((value & 64) << 2) + (value << 1) + (value >> 6)); }
    template<> uint16_t BigCacheRead::convert(int16_t value) { return value <= 0 ? 0 : static_cast<uint16_t>((value << 1) + (value >> 14)); }
    template<> uint16_t BigCacheRead::convert(int32_t value) { return value <= 0 ? 0 : static_cast<uint16_t>(value >> 15); }
    template<> uint16_t BigCacheRead::convert(int64_t value) { return value <= 0 ? 0 : static_cast<uint16_t>(value >> 47); }
    template<> uint16_t BigCacheRead::convert(half value) { return value <= 0 ? 0 : value > 1.0_h ? 0xFFFF : static_cast<uint16_t>(value * 255.0_h + 0.5_h); }
    template<> uint16_t BigCacheRead::convert(float value) { return value <= 0 ? 0 : value > 1.0f ? 0xFFFF : static_cast<uint16_t>(value * 255.0f * 255.0f + 0.5f); }
    template<> uint16_t BigCacheRead::convert(double value) { return value <= 0 ? 0 : value > 1.0 ? 0xFFFF : static_cast<uint16_t>(value * 255.0 * 255.0 + 0.5); }

    template<typename T>
    T BigCacheRead::getElementFromFile(uint64_t entityID, uint64_t index)
    {
        switch (dataTypes[entityID])
        {
        case DataTypes::UINT8_T:
        {
            uint8_t value;
            file.seekg(dataPositions[entityID] + index * sizeof(uint8_t));
            file.read(reinterpret_cast<char*>(&value), sizeof(uint8_t));
            return convert<T, uint8_t>(value);
        }
        case DataTypes::UINT16_T:
        {
            uint16_t value;
            file.seekg(dataPositions[entityID] + index * sizeof(uint16_t));
            file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));
            return convert<T, uint16_t>(value);
        }
        }
        return 0;
    }

    template<typename T>
    T BigCacheRead::getElementFromMemory(uint64_t entityID, uint64_t index)
    {
        switch (dataTypes[entityID])
        {
        case DataTypes::UINT8_T:
            return convert<T, uint8_t>(reinterpret_cast<uint8_t*>(entities[entityID].data.get())[index]);
        case DataTypes::UINT16_T:
            return convert<T, uint16_t>(reinterpret_cast<uint16_t*>(entities[entityID].data.get())[index]);
        }
        return 0;
    }

    template<typename T>
    T BigCacheRead::getElement(uint64_t entityID, uint64_t index)
    {
        if (entities[entityID].data == nullptr) {
            if (entitySizes[entityID] <= maxSize) insert(entityID);
            else return getElementFromFile<T>(entityID, index);
        }
        else {
            if (lru_list.back() != entityID) lru_list.splice(lru_list.end(), lru_list, entities[entityID].it);
        }
        return getElementFromMemory<T>(entityID, index);
    }

    template uint8_t BigCacheRead::getElement(uint64_t entityID, uint64_t index);
    template uint16_t BigCacheRead::getElement(uint64_t entityID, uint64_t index);
}
