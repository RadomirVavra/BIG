#include "../include/big_cache_read.hpp"

#include "../include/half.hpp"

namespace big
{
    using half_float::half;
    using half_float::half_cast;
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

    template<> uint32_t BigCacheRead::convert(uint8_t value) { uint32_t v = static_cast<uint32_t>(value); return (v << 24) + (v << 16) + (v << 8) + v; }
    template<> uint32_t BigCacheRead::convert(uint16_t value) { return (static_cast<uint32_t>(value) << 16) + value; }
    template<> uint32_t BigCacheRead::convert(uint32_t value) { return value; }
    template<> uint32_t BigCacheRead::convert(uint64_t value) { return static_cast<uint32_t>(value >> 32); }
    template<> uint32_t BigCacheRead::convert(int8_t value) { if (value <= 0) return 0; uint32_t v = (value << 1) + (value >> 6); return (v << 24) + (v << 16) + (v << 8) + v; }
    template<> uint32_t BigCacheRead::convert(int16_t value) { if (value <= 0) return 0; uint32_t v = static_cast<uint32_t>((value << 1) + (value >> 14)); return (v << 16) + v; }
    template<> uint32_t BigCacheRead::convert(int32_t value) { return value <= 0 ? 0 : static_cast<uint32_t>((value << 1) + (value >> 30)); }
    template<> uint32_t BigCacheRead::convert(int64_t value) { return value <= 0 ? 0 : static_cast<uint32_t>(value >> 31); }
    template<> uint32_t BigCacheRead::convert(half value) { return value <= 0 ? 0 : value >= 1.0_h ? 0xFFFFFFFF : static_cast<uint32_t>(static_cast<double>(value) * 4294967295.0 + 0.5); }
    template<> uint32_t BigCacheRead::convert(float value) { return value <= 0 ? 0 : value >= 1.0f ? 0xFFFFFFFF : static_cast<uint32_t>(static_cast<double>(value) * 4294967295.0 + 0.5); }
    template<> uint32_t BigCacheRead::convert(double value) { return value <= 0 ? 0 : value >= 1.0 ? 0xFFFFFFFF : static_cast<uint32_t>(value * 4294967295.0 + 0.5); }

    template<> uint64_t BigCacheRead::convert(uint8_t value) { uint64_t v = static_cast<uint64_t>(value); return (v << 56) + (v << 48) + (v << 40) + (v << 32) + (v << 24) + (v << 16) + (v << 8) + v; }
    template<> uint64_t BigCacheRead::convert(uint16_t value) { uint64_t v = static_cast<uint64_t>(value); return (v << 48) + (v << 32) + (v << 16) + v; }
    template<> uint64_t BigCacheRead::convert(uint32_t value) { uint64_t v = static_cast<uint64_t>(value); return (v << 32) + v; }
    template<> uint64_t BigCacheRead::convert(uint64_t value) { return value; }
    template<> uint64_t BigCacheRead::convert(int8_t value) { if (value <= 0) return 0; uint64_t v = static_cast<uint64_t>((value << 1) + (value >> 6)); return (v << 56) + (v << 48) + (v << 40) + (v << 32) + (v << 24) + (v << 16) + (v << 8) + v; }
    template<> uint64_t BigCacheRead::convert(int16_t value) { if (value <= 0) return 0; uint64_t v = static_cast<uint64_t>((value << 1) + (value >> 14)); return (v << 48) + (v << 32) + (v << 16) + v; }
    template<> uint64_t BigCacheRead::convert(int32_t value) { if (value <= 0) return 0; uint64_t v = static_cast<uint64_t>(value); v = static_cast<uint64_t>((v << 1) + (v >> 30)); return (v << 32) + v; }
    template<> uint64_t BigCacheRead::convert(int64_t value) { return value <= 0 ? 0 : static_cast<uint64_t>((value << 1) + (value >> 62)); }
    template<> uint64_t BigCacheRead::convert(half value) { if (value <= 0) return 0; else if (value >= 1.0_h) return 0xFFFFFFFFFFFFFFFF; uint64_t v = static_cast<uint64_t>(static_cast<double>(value) * 9007199254740991.0); return (v << 11) + (v >> 42); }
    template<> uint64_t BigCacheRead::convert(float value) { if (value <= 0) return 0; else if (value >= 1.0f) return 0xFFFFFFFFFFFFFFFF; uint64_t v = static_cast<uint64_t>(static_cast<double>(value) * 9007199254740991.0); return (v << 11) + (v >> 42); }
    template<> uint64_t BigCacheRead::convert(double value) { if (value <= 0) return 0; else if (value >= 1.0) return 0xFFFFFFFFFFFFFFFF; uint64_t v = static_cast<uint64_t>(value * 9007199254740991.0); return (v << 11) + (v >> 42); }

    template<> int8_t BigCacheRead::convert(uint8_t value) { return static_cast<int8_t>(value >> 1); }
    template<> int8_t BigCacheRead::convert(uint16_t value) { return static_cast<int8_t>(value >> 9); }
    template<> int8_t BigCacheRead::convert(uint32_t value) { return static_cast<int8_t>(value >> 25); }
    template<> int8_t BigCacheRead::convert(uint64_t value) { return static_cast<int8_t>(value >> 57); }
    template<> int8_t BigCacheRead::convert(int8_t value) { return value; }
    template<> int8_t BigCacheRead::convert(int16_t value) { return static_cast<int8_t>(value >> 8); }
    template<> int8_t BigCacheRead::convert(int32_t value) { return static_cast<int8_t>(value >> 24); }
    template<> int8_t BigCacheRead::convert(int64_t value) { return static_cast<int8_t>(value >> 56); }
    template<> int8_t BigCacheRead::convert(half value) { return value <= -1.0_h ? -127 : value >= 1.0_h ? 127 : half_cast<int8_t, std::round_to_nearest>(value * 127.0_h); }
    template<> int8_t BigCacheRead::convert(float value) { return value <= -1.0f ? -127 : value >= 1.0f ? 127 : static_cast<int8_t>(round(value * 127.0f)); }
    template<> int8_t BigCacheRead::convert(double value) { return value <= -1.0 ? -127 : value >= 1.0 ? 127 : static_cast<int8_t>(round(value * 127.0)); }

    template<> int16_t BigCacheRead::convert(uint8_t value) { int16_t v = static_cast<int16_t>(value); return (v << 7) | v; }
    template<> int16_t BigCacheRead::convert(uint16_t value) { return static_cast<int16_t>(value >> 1); }
    template<> int16_t BigCacheRead::convert(uint32_t value) { return static_cast<int16_t>(value >> 17); }
    template<> int16_t BigCacheRead::convert(uint64_t value) { return static_cast<int16_t>(value >> 49); }
    template<> int16_t BigCacheRead::convert(int8_t value) { int16_t v = static_cast<int16_t>(value); return (v > 0) ? (v << 8) + (v << 1) + (v >> 6) : (v << 8); }
    template<> int16_t BigCacheRead::convert(int16_t value) { return value; }
    template<> int16_t BigCacheRead::convert(int32_t value) { return static_cast<int16_t>(value >> 16); }
    template<> int16_t BigCacheRead::convert(int64_t value) { return static_cast<int16_t>(value >> 48); }
    template<> int16_t BigCacheRead::convert(half value) { return value <= -1.0_h ? -32767 : value >= 1.0_h ? 32767 : static_cast<int16_t>(round(static_cast<double>(value) * 32767.0)); }
    template<> int16_t BigCacheRead::convert(float value) { return value <= -1.0f ? -32767 : value >= 1.0f ? 32767 : static_cast<int16_t>(round(value * 32767.0f)); }
    template<> int16_t BigCacheRead::convert(double value) { return value <= -1.0 ? -32767 : value >= 1.0 ? 32767 : static_cast<int16_t>(round(value * 32767.0)); }

    template<> int32_t BigCacheRead::convert(uint8_t value) { int32_t v = static_cast<int32_t>(value); return (v << 23) | (v << 15) | (v << 7) | v; }
    template<> int32_t BigCacheRead::convert(uint16_t value) { int32_t v = static_cast<int32_t>(value); return (v << 15) | v; }
    template<> int32_t BigCacheRead::convert(uint32_t value) { return static_cast<int32_t>(value >> 1); }
    template<> int32_t BigCacheRead::convert(uint64_t value) { return static_cast<int32_t>(value >> 33); }
    template<> int32_t BigCacheRead::convert(int8_t value) { int32_t v = static_cast<int32_t>(value); return (v > 0) ? (v << 24) + (v << 17) + (v << 10) + (v << 3) + (v >> 4) : (v << 24); }
    template<> int32_t BigCacheRead::convert(int16_t value) { int32_t v = static_cast<int32_t>(value); return (v > 0) ? (v << 16) + (v << 1) + (v >> 14) : (v << 16); }
    template<> int32_t BigCacheRead::convert(int32_t value) { return value; }
    template<> int32_t BigCacheRead::convert(int64_t value) { return static_cast<int32_t>(value >> 32); }
    template<> int32_t BigCacheRead::convert(half value) { return value <= -1.0_h ? -2147483647 : value >= 1.0_h ? 2147483647 : static_cast<int32_t>(round(static_cast<double>(value) * 2147483647.0)); }
    template<> int32_t BigCacheRead::convert(float value) { return value <= -1.0f ? -2147483647 : value >= 1.0f ? 2147483647 : static_cast<int32_t>(round(static_cast<double>(value) * 2147483647.0)); }
    template<> int32_t BigCacheRead::convert(double value) { return value <= -1.0 ? -2147483647 : value >= 1.0 ? 2147483647 : static_cast<int32_t>(round(value * 2147483647.0)); }

    template<> int64_t BigCacheRead::convert(uint8_t value) { int64_t v = static_cast<int64_t>(value); return (v << 55) | (v << 47) | (v << 39) | (v << 31) | (v << 23) | (v << 15) | (v << 7) | v; }
    template<> int64_t BigCacheRead::convert(uint16_t value) { int64_t v = static_cast<int64_t>(value); return (v << 47) | (v << 31) | (v << 15) | v; }
    template<> int64_t BigCacheRead::convert(uint32_t value) { int64_t v = static_cast<int64_t>(value); return (v << 31) | v; }
    template<> int64_t BigCacheRead::convert(uint64_t value) { return static_cast<int64_t>(value >> 1); }
    template<> int64_t BigCacheRead::convert(int8_t value) { int64_t v = static_cast<int64_t>(value); return (v > 0) ? (v << 56) + (v << 49) + (v << 42) + (v << 35) + (v << 28) + (v << 21) + (v << 14) + (v << 7) + v : (v << 56); }
    template<> int64_t BigCacheRead::convert(int16_t value) { int64_t v = static_cast<int64_t>(value); return (v > 0) ? (v << 48) + (v << 33) + (v << 18) + (v << 3) + (v >> 12) : (v << 48); }
    template<> int64_t BigCacheRead::convert(int32_t value) { int64_t v = static_cast<int64_t>(value); return (v > 0) ? (v << 32) + (v << 1) + (v >> 30) : (v << 32); }
    template<> int64_t BigCacheRead::convert(int64_t value) { return value; }
    template<> int64_t BigCacheRead::convert(half value) { if (value <= -1.0_h) return 0x8000000000000000; else if (value >= 1.0_h) return 0x7FFFFFFFFFFFFFFF; int64_t v = static_cast<int64_t>(round(static_cast<double>(value) * 9007199254740991.0)); return (v << 10) + (v >> 43); }
    template<> int64_t BigCacheRead::convert(float value) { if (value <= -1.0f) return 0x8000000000000000; else if (value >= 1.0f) return 0x7FFFFFFFFFFFFFFF; int64_t v = static_cast<int64_t>(round(static_cast<double>(value) * 9007199254740991.0)); return (v << 10) + (v >> 43); }
    template<> int64_t BigCacheRead::convert(double value) { if (value <= -1.0) return 0x8000000000000000; else if (value >= 1.0) return 0x7FFFFFFFFFFFFFFF; int64_t v = static_cast<int64_t>(round(value * 9007199254740991.0)); return (v << 10) + (v >> 43); }


    template<> float BigCacheRead::convert(uint8_t value) { return static_cast<float>(value) / 255.0f; }
    template<> float BigCacheRead::convert(uint16_t value) { return static_cast<float>(value) / 65535.0f; }
    template<> float BigCacheRead::convert(uint32_t value) { return static_cast<float>(value) / 4294967295.0f; }
    template<> float BigCacheRead::convert(uint64_t value) { return static_cast<float>(value >> 40) / 16777215.0f; }
    template<> float BigCacheRead::convert(int8_t value) { return static_cast<float>(value) / 127.0f; }
    template<> float BigCacheRead::convert(int16_t value) { return static_cast<float>(value) / 32767.0f; }
    template<> float BigCacheRead::convert(int32_t value) { return static_cast<float>(value) / 2147483647.0f; }
    template<> float BigCacheRead::convert(int64_t value) { return static_cast<float>(value) / static_cast<float>(0x7FFFFFFFFFFFFFFF); }
    template<> float BigCacheRead::convert(half value) { return static_cast<float>(value); }
    template<> float BigCacheRead::convert(float value) { return value; }
    template<> float BigCacheRead::convert(double value) { return static_cast<float>(value); }

    template<> double BigCacheRead::convert(uint8_t value) { return static_cast<double>(value) / 255.0; }
    template<> double BigCacheRead::convert(uint16_t value) { return static_cast<double>(value) / 65535.0; }
    template<> double BigCacheRead::convert(uint32_t value) { return static_cast<double>(value) / 4294967295.0; }
    template<> double BigCacheRead::convert(uint64_t value) { return static_cast<double>(value >> 11) / 9007199254740991.0; }
    template<> double BigCacheRead::convert(int8_t value) { return static_cast<double>(value) / 127.0; }
    template<> double BigCacheRead::convert(int16_t value) { return static_cast<double>(value) / 32767.0; }
    template<> double BigCacheRead::convert(int32_t value) { return static_cast<double>(value) / 2147483647.0; }
    template<> double BigCacheRead::convert(int64_t value) { return static_cast<double>(value) / static_cast<double>(0x7FFFFFFFFFFFFFFF); }
    template<> double BigCacheRead::convert(half value) { return static_cast<double>(value); }
    template<> double BigCacheRead::convert(float value) { return static_cast<double>(value); }
    template<> double BigCacheRead::convert(double value) { return value; }



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
        case DataTypes::UINT32_T:
        {
            uint32_t value;
            file.seekg(dataPositions[entityID] + index * sizeof(uint32_t));
            file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
            return convert<T, uint32_t>(value);
        }
        case DataTypes::UINT64_T:
        {
            uint64_t value;
            file.seekg(dataPositions[entityID] + index * sizeof(uint64_t));
            file.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));
            return convert<T, uint64_t>(value);
        }
        case DataTypes::INT8_T:
        {
            int8_t value;
            file.seekg(dataPositions[entityID] + index * sizeof(int8_t));
            file.read(reinterpret_cast<char*>(&value), sizeof(int8_t));
            return convert<T, int8_t>(value);
        }
        case DataTypes::INT16_T:
        {
            int16_t value;
            file.seekg(dataPositions[entityID] + index * sizeof(int16_t));
            file.read(reinterpret_cast<char*>(&value), sizeof(int16_t));
            return convert<T, int16_t>(value);
        }
        case DataTypes::INT32_T:
        {
            int32_t value;
            file.seekg(dataPositions[entityID] + index * sizeof(int32_t));
            file.read(reinterpret_cast<char*>(&value), sizeof(int32_t));
            return convert<T, int32_t>(value);
        }
        case DataTypes::INT64_T:
        {
            int64_t value;
            file.seekg(dataPositions[entityID] + index * sizeof(int64_t));
            file.read(reinterpret_cast<char*>(&value), sizeof(int64_t));
            return convert<T, int64_t>(value);
        }
        /*case DataTypes::HALF: // convert for half not working
        {
            half value;
            file.seekg(dataPositions[entityID] + index * sizeof(half));
            file.read(reinterpret_cast<char*>(&value), sizeof(half));
            return convert<T, half>(value);
        }*/
        case DataTypes::FLOAT:
        {
            float value;
            file.seekg(dataPositions[entityID] + index * sizeof(float));
            file.read(reinterpret_cast<char*>(&value), sizeof(float));
            return convert<T, float>(value);
        }
        case DataTypes::DOUBLE:
        {
            double value;
            file.seekg(dataPositions[entityID] + index * sizeof(double));
            file.read(reinterpret_cast<char*>(&value), sizeof(double));
            return convert<T, double>(value);
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
        case DataTypes::UINT32_T:
            return convert<T, uint32_t>(reinterpret_cast<uint32_t*>(entities[entityID].data.get())[index]);
        case DataTypes::UINT64_T:
            return convert<T, uint64_t>(reinterpret_cast<uint64_t*>(entities[entityID].data.get())[index]);
        case DataTypes::INT8_T:
            return convert<T, int8_t>(reinterpret_cast<int8_t*>(entities[entityID].data.get())[index]);
        case DataTypes::INT16_T:
            return convert<T, int16_t>(reinterpret_cast<int16_t*>(entities[entityID].data.get())[index]);
        case DataTypes::INT32_T:
            return convert<T, int32_t>(reinterpret_cast<int32_t*>(entities[entityID].data.get())[index]);
        case DataTypes::INT64_T:
            return convert<T, int64_t>(reinterpret_cast<int64_t*>(entities[entityID].data.get())[index]);
       /* case DataTypes::HALF:
            return convert<T, half>(reinterpret_cast<half*>(entities[entityID].data.get())[index]); convert for half missing*/ 
        case DataTypes::FLOAT:
            return convert<T, float>(reinterpret_cast<float*>(entities[entityID].data.get())[index]);
        case DataTypes::DOUBLE:
            return convert<T, double>(reinterpret_cast<double*>(entities[entityID].data.get())[index]);
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