#include "common.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace big_test
{
    void checkHeader(std::ifstream &file)
    {
        std::string header(big::CHUNK_LENGTH, ' ');
        file.read(&header[0], big::CHUNK_LENGTH);
        Assert::AreEqual(big::MAGIC, header);
    }

    void checkMetaData(std::ifstream &file, const std::vector<uint64_t> &metaData)
    {
        for (const auto & d : metaData) {
            uint64_t number;
            file.read(reinterpret_cast<char*>(&number), big::CHUNK_LENGTH);
            Assert::AreEqual(d, number);
        }
    }

    template <typename T>
    void checkData(std::ifstream &file, std::shared_ptr<T> data, uint64_t n, uint64_t index, uint64_t dataType)
    {
        uint64_t number;
        file.read(reinterpret_cast<char*>(&number), big::CHUNK_LENGTH);
        Assert::AreEqual(6ull, number);
        file.read(reinterpret_cast<char*>(&number), big::CHUNK_LENGTH);
        uint64_t complement = (sizeof(T) * n) % big::CHUNK_LENGTH > 0 ? big::CHUNK_LENGTH - (sizeof(T) * n) % big::CHUNK_LENGTH : 0;
        uint64_t length = 2*big::CHUNK_LENGTH + sizeof(T) * n + complement;
        Assert::AreEqual(length, number);
        file.read(reinterpret_cast<char*>(&number), big::CHUNK_LENGTH);
        Assert::AreEqual(index, number);
		file.read(reinterpret_cast<char*>(&number), big::CHUNK_LENGTH);
		Assert::AreEqual(dataType, number);
        for (uint64_t i = 0; i != n; ++i) {
            T number;
            file.read(reinterpret_cast<char*>(&number), sizeof(T));
            Assert::AreEqual(data.get()[i], number);
        }
        uint64_t zero = 0;
        file.read(reinterpret_cast<char*>(&zero), complement);
        Assert::AreEqual(0ull, zero);
    }

    template void checkData(std::ifstream &file, std::shared_ptr<uint8_t> data, uint64_t n, uint64_t index, uint64_t dataType);
    template void checkData(std::ifstream &file, std::shared_ptr<uint16_t> data, uint64_t n, uint64_t index, uint64_t dataType);
    template void checkData(std::ifstream &file, std::shared_ptr<uint32_t> data, uint64_t n, uint64_t index, uint64_t dataType);
    template void checkData(std::ifstream &file, std::shared_ptr<uint64_t> data, uint64_t n, uint64_t index, uint64_t dataType);
    template void checkData(std::ifstream &file, std::shared_ptr<int8_t> data, uint64_t n, uint64_t index, uint64_t dataType);
    template void checkData(std::ifstream &file, std::shared_ptr<int16_t> data, uint64_t n, uint64_t index, uint64_t dataType);
    template void checkData(std::ifstream &file, std::shared_ptr<int32_t> data, uint64_t n, uint64_t index, uint64_t dataType);
    template void checkData(std::ifstream &file, std::shared_ptr<int64_t> data, uint64_t n, uint64_t index, uint64_t dataType);
    //template void checkData(std::ifstream &file, std::shared_ptr<half> data, uint64_t n, uint64_t index);
    template void checkData(std::ifstream &file, std::shared_ptr<float> data, uint64_t n, uint64_t index, uint64_t dataType);
    template void checkData(std::ifstream &file, std::shared_ptr<double> data, uint64_t n, uint64_t index, uint64_t dataType);

    void writeHeader(std::ofstream &file)
    {
        file.write(&big::MAGIC[0], big::CHUNK_LENGTH);
    }

    void writeMetaData(std::ofstream &file, const std::vector<uint64_t> &metaData)
    {
        for (const auto & d : metaData) {
            file.write(reinterpret_cast<const char*>(&d), big::CHUNK_LENGTH);
        }
    }

    template <typename T>
    void writeData(std::ofstream &file, std::shared_ptr<T> data, uint64_t n, uint64_t index)
    {
        uint64_t number = 7ull;
        file.write(reinterpret_cast<char*>(&number), big::CHUNK_LENGTH);
        uint64_t complement = (sizeof(T) * n) % big::CHUNK_LENGTH > 0 ? big::CHUNK_LENGTH - (sizeof(T) * n) % big::CHUNK_LENGTH : 0;
        uint64_t length = big::CHUNK_LENGTH + sizeof(T) * n + complement;
        file.write(reinterpret_cast<char*>(&length), big::CHUNK_LENGTH);
        file.write(reinterpret_cast<char*>(&index), big::CHUNK_LENGTH);
        for (uint64_t i = 0; i != n; ++i) {
            file.write(reinterpret_cast<char*>(&data.get()[i]), sizeof(T));
        }
        uint64_t zero = 0;
        file.write(reinterpret_cast<char*>(&zero), complement);
    }

    template void writeData(std::ofstream &file, std::shared_ptr<uint8_t> data, uint64_t n, uint64_t index);
    template void writeData(std::ofstream &file, std::shared_ptr<uint16_t> data, uint64_t n, uint64_t index);
    template void writeData(std::ofstream &file, std::shared_ptr<uint32_t> data, uint64_t n, uint64_t index);
    template void writeData(std::ofstream &file, std::shared_ptr<uint64_t> data, uint64_t n, uint64_t index);
    template void writeData(std::ofstream &file, std::shared_ptr<int8_t> data, uint64_t n, uint64_t index);
    template void writeData(std::ofstream &file, std::shared_ptr<int16_t> data, uint64_t n, uint64_t index);
    template void writeData(std::ofstream &file, std::shared_ptr<int32_t> data, uint64_t n, uint64_t index);
    template void writeData(std::ofstream &file, std::shared_ptr<int64_t> data, uint64_t n, uint64_t index);
    //template void writeData(std::ofstream &file, std::shared_ptr<half> data, uint64_t n, uint64_t index);
    template void writeData(std::ofstream &file, std::shared_ptr<float> data, uint64_t n, uint64_t index);
    template void writeData(std::ofstream &file, std::shared_ptr<double> data, uint64_t n, uint64_t index);
}
