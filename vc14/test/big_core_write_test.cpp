#include "common.hpp"

#include "../../include/big_core_write.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace big_test
{
    TEST_CLASS(BigCoreWrite_UnitTest)
    {
    public:

        TEST_METHOD(BigCoreWrite_Constructor)
        {
            {
                {
                    big::BigCoreWrite big("testCoreWrite_Constructor1.big", 1, 2, 4);

                    Assert::AreEqual(1ull, big.getNumberOfImages());
                    Assert::AreEqual(2ull, big.getImageHeight());
                    Assert::AreEqual(4ull, big.getImageWidth());
                    Assert::AreEqual(1ull, big.getNumberOfPlanes());
                }
                {
                    std::ifstream file("testCoreWrite_Constructor1.big", std::ios_base::binary | std::ios_base::in);
                    checkHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 1, 2, 8, 2, 3, 8, 4, 4, 8, 1, 5, 32, 1, 2, 3, 4, 6, 8, 1 };
                    checkMetaData(file, metaData);
                }
            }
            {
                {
                    big::BigCoreWrite big("testCoreWrite_Constructor2.big", 2, 3, 5, 3);
                }
                {
                    std::ifstream file("testCoreWrite_Constructor2.big", std::ios_base::binary | std::ios_base::in);
                    checkHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 1, 2, 3, 4, 6, 8, 1 };
                    checkMetaData(file, metaData);
                }
            }
            {
                std::vector<big::ChunkIds> dataOrder = { big::ChunkIds::IMAGE_HEIGHT, big::ChunkIds::IMAGE_WIDTH, big::ChunkIds::NUMBER_OF_PLANES, big::ChunkIds::NUMBER_OF_IMAGES };
                std::vector<big::DataTypes> dataType = { big::DataTypes::UINT16_T };
                {
                    big::BigCoreWrite big("testCoreWrite_Constructor3.big", 2, 3, 5, 3, dataOrder, dataType);
                }
                {
                    std::ifstream file("testCoreWrite_Constructor3.big", std::ios_base::binary | std::ios_base::in);
                    checkHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 2, 3, 4, 1, 6, 8, 2 };
                    checkMetaData(file, metaData);
                }
            }
        }

        TEST_METHOD(BigCoreWrite_addEntity)
        {
            {
                uint64_t n = 2 * 4;
                std::shared_ptr<uint8_t> data{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (int i = 0; i != n; ++i) data.get()[i] = i;
                {
                    big::BigCoreWrite big("testCoreWrite_AddEntity1.big", 1, 2, 4);
                    big.addEntity(data, 0);
                }
                {
                    std::ifstream file("testCoreWrite_AddEntity1.big", std::ios_base::binary | std::ios_base::in);
                    checkHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 1, 2, 8, 2, 3, 8, 4, 4, 8, 1, 5, 32, 1, 2, 3, 4, 6, 8, 1 };
                    checkMetaData(file, metaData);
                    checkData(file, data, n, 0);
                }
            }
            {
                uint64_t n = 3 * 5 * 3;
                std::shared_ptr<uint8_t> data1{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data1.get()[i] = i;
                std::shared_ptr<uint8_t> data2{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint8_t>(n) + i;
                {
                    big::BigCoreWrite big("testCoreWrite_AddEntity2.big", 2, 3, 5, 3);
                    big.addEntity(data1, 0);
                    big.addEntity(data2, 1);
                }
                {
                    std::ifstream file("testCoreWrite_AddEntity2.big", std::ios_base::binary | std::ios_base::in);
                    checkHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 1, 2, 3, 4, 6, 8, 1 };
                    checkMetaData(file, metaData);
                    checkData(file, data1, n, 0);
                    checkData(file, data2, n, 1);
                }
            }
            {
                std::vector<big::ChunkIds> dataOrder = { big::ChunkIds::IMAGE_HEIGHT, big::ChunkIds::IMAGE_WIDTH, big::ChunkIds::NUMBER_OF_PLANES, big::ChunkIds::NUMBER_OF_IMAGES };
                std::vector<big::DataTypes> dataType = { big::DataTypes::UINT16_T };
                uint16_t n = 5 * 3 * 2;
                std::shared_ptr<uint16_t> data1{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data1.get()[i] = i;
                std::shared_ptr<uint16_t> data2{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint16_t>(n) + i;
                std::shared_ptr<uint16_t> data3{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data3.get()[i] = 2 * static_cast<uint16_t>(n) + i;
                {
                    big::BigCoreWrite big("testCoreWrite_AddEntity3.big", 2, 3, 5, 3, dataOrder, dataType);
                    big.addEntity(data1, 0);
                    big.addEntity(data2, 1);
                    big.addEntity(data3, 2);
                }
                {
                    std::ifstream file("testCoreWrite_AddEntity3.big", std::ios_base::binary | std::ios_base::in);
                    checkHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 2, 3, 4, 1, 6, 8, 2 };
                    checkMetaData(file, metaData);
                    checkData(file, data1, n, 0);
                    checkData(file, data2, n, 1);
                    checkData(file, data3, n, 2);
                }
            }
        }
    };
}
