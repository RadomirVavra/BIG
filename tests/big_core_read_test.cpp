#include "common.hpp"

#include "../include/big_core_read.hpp"
#include "../include/big_core_write.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace big_test
{		
	TEST_CLASS(BigCoreRead_UnitTest)
	{
	public:
		
        TEST_METHOD(BigCoreRead_GetAndAt)
        {
            {
                uint64_t n = 2 * 4;
                std::shared_ptr<uint8_t> data{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (int i = 0; i != n; ++i) data.get()[i] = i;
                {
                    //std::ofstream file("testCoreRead_Get1.big", std::ios_base::binary | std::ios_base::out);
                    //writeHeader(file);
                    //std::vector<uint64_t> metaData{ 1, 8, 1, 2, 8, 2, 3, 8, 4, 4, 8, 1, 5, 32, 1, 2, 3, 4,  };//6, 8, 1
                    //writeMetaData(file, metaData);
                    //writeData(file, data, n, 0);
					big::BigCoreWrite big("testCoreRead_Get1.big", 2, 4, 1 ); // default data order
					big.addEntity(data, 0, big::DataTypes::UINT8_T);
                }
                {
                    big::BigCoreRead big("testCoreRead_Get1.big");
                    Assert::AreEqual(1ull, big.getNumberOfImages());
                    Assert::AreEqual(2ull, big.getImageHeight());
                    Assert::AreEqual(4ull, big.getImageWidth());
                    Assert::AreEqual(1ull, big.getNumberOfPlanes());
                    const auto &dataOrder = big.getDataOrder();
                    for (uint64_t i = 0; i != dataOrder.size(); ++i) {
                        Assert::AreEqual(big::defaultDataOrder[i], dataOrder[i]);
                    }
                    /*const auto &dataType = big.getDataType();
                    for (uint64_t i = 0; i != dataType.size(); ++i) {
                        Assert::AreEqual(big::defaultDataType[i], dataType[i]);
                    }*/
                    uint64_t index = 0;
                    uint64_t imageNum = 0;
                    uint64_t plane = 0;
                    for (uint64_t row = 0; row != 2; ++row) {
                        for (uint64_t col = 0; col != 4; ++col) {
                            uint8_t d = big.at<uint8_t>(imageNum, row, col);
                            Assert::AreEqual(data.get()[index], d);
                            d = big.get<uint8_t>(imageNum, row, col);
                            Assert::AreEqual(data.get()[index], d);
                            ++index;
                        }
                    }
                }
            }
            {
                uint64_t n = 3 * 5 * 3;
                std::shared_ptr<uint8_t> data1{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data1.get()[i] = i;
                std::shared_ptr<uint8_t> data2{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint8_t>(n) + i;
                {
                    /*std::ofstream file("testCoreRead_Get2.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 1, 2, 3, 4, 6, 8, 1 };
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);*/
					
                }
                {
                    big::BigCoreRead big("testCoreRead_Get2.big");
                    Assert::AreEqual(2ull, big.getNumberOfImages());
                    Assert::AreEqual(3ull, big.getImageHeight());
                    Assert::AreEqual(5ull, big.getImageWidth());
                    Assert::AreEqual(3ull, big.getNumberOfPlanes());
                    const auto &dataOrder = big.getDataOrder();
                    for (uint64_t i = 0; i != dataOrder.size(); ++i) {
                        Assert::AreEqual(big::defaultDataOrder[i], dataOrder[i]);
                    }
                    const auto &dataType = big.getDataType();
                    for (uint64_t i = 0; i != dataType.size(); ++i) {
                        Assert::AreEqual(big::defaultDataType[0], dataType[i]);
                    }
                    std::shared_ptr<uint8_t> data = data1;
                    uint64_t index = 0;
                    for (uint64_t imageNum = 0; imageNum != 2; ++imageNum) {
                        for (uint64_t row = 0; row != 3; ++row) {
                            for (uint64_t col = 0; col != 5; ++col) {
                                for (uint64_t plane = 0; plane != 3; ++plane) {
                                    uint8_t d = big.at<uint8_t>(imageNum, row, col, plane);
                                    Assert::AreEqual(data.get()[index % n], d);
                                    d = big.get<uint8_t>(imageNum, row, col, plane);
                                    Assert::AreEqual(data.get()[index % n], d);
                                    ++index;
                                    if (index == n) data = data2;
                                }
                            }
                        }
                    }
                }
            }
            {
                std::vector<big::ChunkIds> dataOrder = { big::ChunkIds::IMAGE_HEIGHT, big::ChunkIds::IMAGE_WIDTH, big::ChunkIds::NUMBER_OF_PLANES, big::ChunkIds::NUMBER_OF_IMAGES };
                std::vector<big::DataTypes> dataType = { big::DataTypes::UINT16_T };
                uint16_t n = 5 * 3 * 2;
                std::shared_ptr<uint16_t> data1{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data1.get()[i] = 10 * i;
                std::shared_ptr<uint16_t> data2{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint16_t>(n) + i;
                std::shared_ptr<uint16_t> data3{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data3.get()[i] = 2 * static_cast<uint16_t>(n) + i;
                {
                    std::ofstream file("testCoreRead_Get3.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32 };// , 1, 2, 3, 4, 6, 8, 1 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataOrder) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    metaData = { 6, 8 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataType) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);
                    writeData(file, data3, n, 2);
                }
				{
					//catch exception, which is throw if dataorder is not correct
					try
					{
						big::BigCoreRead big("testCoreRead_Get3.big");
					
						Assert::AreEqual(2ull, big.getNumberOfImages());
						Assert::AreEqual(3ull, big.getImageHeight());
						Assert::AreEqual(5ull, big.getImageWidth());
						Assert::AreEqual(3ull, big.getNumberOfPlanes());
					
							const auto &dataOrder2 = big.getDataOrder();
					
					
					
					    for (uint64_t i = 0; i != dataOrder2.size(); ++i) { 
							Assert::AreEqual(dataOrder[i], dataOrder2[i]);
						}
						const auto &dataType2 = big.getDataType();
						for (uint64_t i = 0; i != dataType2.size(); ++i) {
							Assert::AreEqual(dataType[i], dataType2[i]);
						}
						std::shared_ptr<uint16_t> data = data1;
						uint64_t index = 0;
						for (uint64_t row = 0; row != 3; ++row) {
							for (uint64_t col = 0; col != 5; ++col) {
								for (uint64_t plane = 0; plane != 3; ++plane) {
									for (uint64_t imageNum = 0; imageNum != 2; ++imageNum) {
										uint16_t d = big.at<uint16_t>(imageNum, row, col, plane);
										Assert::AreEqual(data.get()[index % n], d);
										d = big.get<uint16_t>(imageNum, row, col, plane);
										Assert::AreEqual(data.get()[index % n], d);
										++index;
										if (index == n) data = data2;
										else if (index == 2 * n) data = data3;
									}
								}
							}
						}
					}
					catch (const char * str)
					{
						Assert::AreEqual(str, "This data order is not supported!");
					}
                }
            }
        }

        TEST_METHOD(BigCoreRead_SmallCache) //Cache have size of one entity
        {
            {
                uint64_t n = 3 * 5 * 3;
                std::shared_ptr<uint8_t> data1{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data1.get()[i] = i;
                std::shared_ptr<uint8_t> data2{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint8_t>(n) + i;
                {
                    /*std::ofstream file("testCoreRead_SmallCache1.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 1, 2, 3, 4}; , 6, 8, 1 
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);*/
					big::BigCoreWrite big("testCoreRead_SmallCache1.big", 3, 5, 3); // default data order
					big.addEntity(data1, 0, big::DataTypes::UINT8_T);
					big.addEntity(data2, 1, big::DataTypes::UINT8_T);

                }
                {
                    big::BigCoreRead big("testCoreRead_SmallCache1.big", false, n);
                    Assert::AreEqual(2ull, big.getNumberOfImages());
                    Assert::AreEqual(3ull, big.getImageHeight());
                    Assert::AreEqual(5ull, big.getImageWidth());
                    Assert::AreEqual(3ull, big.getNumberOfPlanes());
                    const auto &dataOrder = big.getDataOrder();
                    for (uint64_t i = 0; i != dataOrder.size(); ++i) {
                        Assert::AreEqual(big::defaultDataOrder[i], dataOrder[i]);
                    }
                    const auto &dataType = big.getDataType();
                    for (uint64_t i = 0; i != dataType.size(); ++i) {
                        Assert::AreEqual(big::defaultDataType[0], dataType[i]);
                    }
                    std::shared_ptr<uint8_t> data = data1;
                    uint64_t index = 0;
                    for (uint64_t imageNum = 0; imageNum != 2; ++imageNum) {
                        for (uint64_t row = 0; row != 3; ++row) {
                            for (uint64_t col = 0; col != 5; ++col) {
                                for (uint64_t plane = 0; plane != 3; ++plane) {
                                    uint8_t d = big.get<uint8_t>(imageNum, row, col, plane);
                                    Assert::AreEqual(data.get()[index % n], d);
                                    ++index;
                                    if (index == n) data = data2;
                                }
                            }
                        }
                    }
                }
            }
            {

				//this test throw exception, is not upgrade to new datatype
                std::vector<big::ChunkIds> dataOrder = { big::ChunkIds::IMAGE_HEIGHT, big::ChunkIds::IMAGE_WIDTH, big::ChunkIds::NUMBER_OF_PLANES, big::ChunkIds::NUMBER_OF_IMAGES };
                std::vector<big::DataTypes> dataType = { big::DataTypes::UINT16_T };
                uint16_t n = 5 * 3 * 2;
                std::shared_ptr<uint16_t> data1{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data1.get()[i] = 10 * i;
                std::shared_ptr<uint16_t> data2{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint16_t>(n) + i;
                std::shared_ptr<uint16_t> data3{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data3.get()[i] = 2 * static_cast<uint16_t>(n) + i;
                {
                    std::ofstream file("testCoreRead_SmallCache2.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32 };// , 1, 2, 3, 4, 6, 8, 1 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataOrder) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    metaData = { 6, 8 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataType) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);
                    writeData(file, data3, n, 2);
                }
                {
					//catch exception, which is throw if dataorder is not correct
					try {

					
						big::BigCoreRead big("testCoreRead_SmallCache2.big", false, sizeof(uint16_t) * n);
				

						Assert::AreEqual(2ull, big.getNumberOfImages());
						Assert::AreEqual(3ull, big.getImageHeight());
						Assert::AreEqual(5ull, big.getImageWidth());
						Assert::AreEqual(3ull, big.getNumberOfPlanes());
						const auto &dataOrder2 = big.getDataOrder();
						for (uint64_t i = 0; i != dataOrder2.size(); ++i) {
							Assert::AreEqual(dataOrder[i], dataOrder2[i]);
						}
						const auto &dataType2 = big.getDataType();
						for (uint64_t i = 0; i != dataType2.size(); ++i) {
							Assert::AreEqual(dataType[i], dataType2[i]);
						}
						for (uint64_t imageNum = 0; imageNum != 2; ++imageNum) {
							for (uint64_t row = 0; row != 3; ++row) {
								for (uint64_t col = 0; col != 5; ++col) {
									for (uint64_t plane = 0; plane != 3; ++plane) {
										uint64_t index = row * 5 * 3 * 2 + col * 3 * 2 + plane * 2 + imageNum;
										std::shared_ptr<uint16_t> data;
										if (index / n == 0) data = data1;
										else if (index / n == 1) data = data2;
										else if (index / n == 2) data = data3;
										uint16_t d = big.get<uint16_t>(imageNum, row, col, plane);
										Assert::AreEqual(data.get()[index % n], d);
									}
								}
							}
						}
					}
					catch (const char * str)
					{
						Assert::AreEqual(str, "This data order is not supported!");
					}
                }
            }
        }
        TEST_METHOD(BigCoreRead_ZeroCache)
        {
            {
				std::vector<std::shared_ptr<uint8_t>> data;
                uint64_t n = 3 * 5 * 3;
                std::shared_ptr<uint8_t> data1{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data1.get()[i] = i;
				data.push_back(data1);
                std::shared_ptr<uint8_t> data2{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint8_t>(n) + i;
				data.push_back(data2);
                {
                    /*std::ofstream file("testCoreRead_ZeroCache1.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 1, 2, 3, 4, 6, 8, 1 };
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);*/
					big::BigCoreWrite big("testCoreRead_ZeroCache1.big", 3, 5, 3); // default data order
					big.addEntity(data1, 0, big::DataTypes::UINT8_T);
					big.addEntity(data2, 1, big::DataTypes::UINT8_T);
                }
                {
                    big::BigCoreRead big("testCoreRead_ZeroCache1.big", false, 0);
                    Assert::AreEqual(2ull, big.getNumberOfImages());
                    Assert::AreEqual(3ull, big.getImageHeight());
                    Assert::AreEqual(5ull, big.getImageWidth());
                    Assert::AreEqual(3ull, big.getNumberOfPlanes());
                    const auto &dataOrder = big.getDataOrder();
                    for (uint64_t i = 0; i != dataOrder.size(); ++i) {
                        Assert::AreEqual(big::defaultDataOrder[i], dataOrder[i]);
                    }
                    const auto &dataType = big.getDataType();
                    for (uint64_t i = 0; i != dataType.size(); ++i) {
                        Assert::AreEqual(big::defaultDataType[0], dataType[i]);
                    }
					
                    uint64_t index = 0;
                    for (uint64_t imageNum = 0; imageNum != 2; ++imageNum) {
						checkAtandGet(big, data[imageNum], imageNum, 3, 5, 3, n);
                        /*for (uint64_t row = 0; row != 3; ++row) {
                            for (uint64_t col = 0; col != 5; ++col) {
                                for (uint64_t plane = 0; plane != 3; ++plane) {
                                    uint8_t d = big.get<uint8_t>(imageNum, row, col, plane);
                                    Assert::AreEqual(data.get()[index % n], d);
                                    ++index;
                                    if (index == n) data = data2;
                                }
                            }
                        }*/
                    }
					
                }
            }
            {
                std::vector<big::ChunkIds> dataOrder = { big::ChunkIds::IMAGE_HEIGHT, big::ChunkIds::IMAGE_WIDTH, big::ChunkIds::NUMBER_OF_PLANES, big::ChunkIds::NUMBER_OF_IMAGES };
                std::vector<big::DataTypes> dataType = { big::DataTypes::UINT16_T };
                uint16_t n = 5 * 3 * 2;
                std::shared_ptr<uint16_t> data1{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data1.get()[i] = 10 * i;
                std::shared_ptr<uint16_t> data2{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint16_t>(n) + i;
                std::shared_ptr<uint16_t> data3{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data3.get()[i] = 2 * static_cast<uint16_t>(n) + i;
                {
                    std::ofstream file("testCoreRead_ZeroCache2.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32 };// , 1, 2, 3, 4, 6, 8, 1 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataOrder) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    metaData = { 6, 8 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataType) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);
                    writeData(file, data3, n, 2);
                }
                {
					//catch exception, which is throw if dataorder is not correct
					try
					{
						big::BigCoreRead big("testCoreRead_ZeroCache2.big", false, 0);
						Assert::AreEqual(2ull, big.getNumberOfImages());
						Assert::AreEqual(3ull, big.getImageHeight());
						Assert::AreEqual(5ull, big.getImageWidth());
						Assert::AreEqual(3ull, big.getNumberOfPlanes());
						const auto &dataOrder2 = big.getDataOrder();
						for (uint64_t i = 0; i != dataOrder2.size(); ++i) {
							Assert::AreEqual(dataOrder[i], dataOrder2[i]);
						}
						const auto &dataType2 = big.getDataType();
						for (uint64_t i = 0; i != dataType2.size(); ++i) {
							Assert::AreEqual(dataType[i], dataType2[i]);
						}
						for (uint64_t imageNum = 0; imageNum != 2; ++imageNum) {
							for (uint64_t row = 0; row != 3; ++row) {
								for (uint64_t col = 0; col != 5; ++col) {
									for (uint64_t plane = 0; plane != 3; ++plane) {
										uint64_t index = row * 5 * 3 * 2 + col * 3 * 2 + plane * 2 + imageNum;
										std::shared_ptr<uint16_t> data;
										if (index / n == 0) data = data1;
										else if (index / n == 1) data = data2;
										else if (index / n == 2) data = data3;
										uint16_t d = big.get<uint16_t>(imageNum, row, col, plane);
										Assert::AreEqual(data.get()[index % n], d);
									}
								}
							}
						}
					}
					catch (const char * str)
					{
						Assert::AreEqual(str, "This data order is not supported!");
					}
                }
            }
        }


        TEST_METHOD(BigCoreRead_CacheTest)
        {
            {
                uint64_t n = 3 * 5 * 3;
                std::shared_ptr<uint8_t> data1{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data1.get()[i] = i;
                std::shared_ptr<uint8_t> data2{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint8_t>(n) + i;
                {
                    /*std::ofstream file("testCoreRead_CacheTest1.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 1, 2, 3, 4 };//, 6, 8, 1
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);*/
					big::BigCoreWrite big("testCoreRead_CacheTest1.big", 3, 5, 3); // default data order
					big.addEntity(data1, 0, big::DataTypes::UINT8_T);
					big.addEntity(data2, 1, big::DataTypes::UINT8_T);
                }
                {
                    big::BigCoreRead big("testCoreRead_CacheTest1.big", true);
                    Assert::AreEqual(true, big.isAllInCache());
                    Assert::AreEqual(2 * n, big.getCacheSize());
                    big.clearCache();
                    Assert::AreEqual(false, big.isAllInCache());
                    Assert::AreEqual(0ull, big.getCacheSize());
                    big.setCacheSize(0);
                    big.loadToCache();
                    Assert::AreEqual(false, big.isAllInCache());
                    Assert::AreEqual(0ull, big.getCacheSize());
                    big.loadToCache(2 * n);
                    Assert::AreEqual(true, big.isAllInCache());
                    Assert::AreEqual(2 * n, big.getCacheSize());
                    big.setCacheSize(n);
                    Assert::AreEqual(true, big.isAllInCache());
                    Assert::AreEqual(2 * n, big.getCacheSize());
                    big.shrinkCache();
                    Assert::AreEqual(false, big.isAllInCache());
                    Assert::AreEqual(n, big.getCacheSize());
                    big.setCacheSize(2 * n);
                    Assert::AreEqual(false, big.isAllInCache());
                    Assert::AreEqual(n, big.getCacheSize());
                }
            }
        }

        TEST_METHOD(BigCoreRead_Operator)
        {
            {
                uint64_t n = 3 * 5 * 3;
                std::shared_ptr<uint8_t> data1{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data1.get()[i] = i;
                std::shared_ptr<uint8_t> data2{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint8_t>(n) + i;
                {
                   /* std::ofstream file("testCoreRead_Operator1.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 1, 2, 3, 4, 6, 8, 1 };
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);*/
					big::BigCoreWrite big("testCoreRead_Operator1.big", 3, 5, 3); // default data order
					big.addEntity(data1, 0, big::DataTypes::UINT8_T);
					big.addEntity(data2, 1, big::DataTypes::UINT8_T);
                }
                {
                    big::BigCoreRead big("testCoreRead_Operator1.big", false, n);
                    Assert::AreEqual(2ull, big.getNumberOfImages());
                    Assert::AreEqual(3ull, big.getImageHeight());
                    Assert::AreEqual(5ull, big.getImageWidth());
                    Assert::AreEqual(3ull, big.getNumberOfPlanes());
                    const auto &dataOrder = big.getDataOrder();
                    for (uint64_t i = 0; i != dataOrder.size(); ++i) {
                        Assert::AreEqual(big::defaultDataOrder[i], dataOrder[i]);
                    }
                    const auto &dataType = big.getDataType();
                    for (uint64_t i = 0; i != dataType.size(); ++i) {
                        Assert::AreEqual(big::defaultDataType[0], dataType[i]);
                    }

                    auto d1 = big[0];
                    uint64_t index = 0;
                    uint64_t imageNum = 0;
                    for (uint64_t row = 0; row != 3; ++row) {
                        for (uint64_t col = 0; col != 5; ++col) {
                            for (uint64_t plane = 0; plane != 3; ++plane) {
                                Assert::AreEqual(data1.get()[index], reinterpret_cast<const uint8_t*>(d1.get())[index]);
                                ++index;
                            }
                        }
                    }
                    auto d2 = big[1];
                    index = 0;
                    imageNum = 1;
                    for (uint64_t row = 0; row != 3; ++row) {
                        for (uint64_t col = 0; col != 5; ++col) {
                            for (uint64_t plane = 0; plane != 3; ++plane) {
                                Assert::AreEqual(data2.get()[index], reinterpret_cast<const uint8_t*>(d2.get())[index]);
                                ++index;
                            }
                        }
                    }
                    index = 0;
                    imageNum = 0;
                    for (uint64_t row = 0; row != 3; ++row) {
                        for (uint64_t col = 0; col != 5; ++col) {
                            for (uint64_t plane = 0; plane != 3; ++plane) {
                                Assert::AreEqual(data1.get()[index], reinterpret_cast<const uint8_t*>(d1.get())[index]);
                                ++index;
                            }
                        }
                    }
                }
                {
                    big::BigCoreRead big("testCoreRead_Operator1.big", false, 0);
                    Assert::AreEqual(2ull, big.getNumberOfImages());
                    Assert::AreEqual(3ull, big.getImageHeight());
                    Assert::AreEqual(5ull, big.getImageWidth());
                    Assert::AreEqual(3ull, big.getNumberOfPlanes());
                    const auto &dataOrder = big.getDataOrder();
                    for (uint64_t i = 0; i != dataOrder.size(); ++i) {
                        Assert::AreEqual(big::defaultDataOrder[i], dataOrder[i]);
                    }
                    const auto &dataType = big.getDataType();
                    for (uint64_t i = 0; i != dataType.size(); ++i) {
                        Assert::AreEqual(big::defaultDataType[0], dataType[i]);
                    }

                    auto d1 = big[0];
                    uint64_t index = 0;
                    uint64_t imageNum = 0;
                    for (uint64_t row = 0; row != 3; ++row) {
                        for (uint64_t col = 0; col != 5; ++col) {
                            for (uint64_t plane = 0; plane != 3; ++plane) {
                                Assert::AreEqual(data1.get()[index], reinterpret_cast<const uint8_t*>(d1.get())[index]);
                                ++index;
                            }
                        }
                    }
                    auto d2 = big[1];
                    index = 0;
                    imageNum = 1;
                    for (uint64_t row = 0; row != 3; ++row) {
                        for (uint64_t col = 0; col != 5; ++col) {
                            for (uint64_t plane = 0; plane != 3; ++plane) {
                                Assert::AreEqual(data2.get()[index], reinterpret_cast<const uint8_t*>(d2.get())[index]);
                                ++index;
                            }
                        }
                    }
                    index = 0;
                    imageNum = 0;
                    for (uint64_t row = 0; row != 3; ++row) {
                        for (uint64_t col = 0; col != 5; ++col) {
                            for (uint64_t plane = 0; plane != 3; ++plane) {
                                Assert::AreEqual(data1.get()[index], reinterpret_cast<const uint8_t*>(d1.get())[index]);
                                ++index;
                            }
                        }
                    }
                }
            }
        }



        TEST_METHOD(BigCoreRead_GetEntity)
        {
            {
                uint64_t n = 2 * 4;
                std::shared_ptr<uint8_t> data{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (int i = 0; i != n; ++i) data.get()[i] = i;
                {
                    //std::ofstream file("testCoreRead_GetEntity1.big", std::ios_base::binary | std::ios_base::out);
                    //writeHeader(file);
                    //std::vector<uint64_t> metaData{ 1, 8, 1, 2, 8, 2, 3, 8, 4, 4, 8, 1, 5, 32, 1, 2, 3, 4 }; //6, 8, 1
                    //writeMetaData(file, metaData);
                    //writeData(file, data, n, 0);
					big::BigCoreWrite big("testCoreRead_GetEntity1.big", 2, 4, 1); // default data order
					big.addEntity(data, 0, big::DataTypes::UINT8_T);
                }
                {
                    big::BigCoreRead big("testCoreRead_GetEntity1.big");
                    auto vec = big.getEntity<uint8_t>(0);
                    uint64_t index = 0;
                    for (uint64_t row = 0; row != 2; ++row) {
                        for (uint64_t col = 0; col != 4; ++col) {
                            Assert::AreEqual(data.get()[index], vec[index]);
                            ++index;
                        }
                    }
                }
                {
                    big::BigCoreRead big("testCoreRead_GetEntity1.big", false, 0);
                    auto vec = big.getEntity<uint8_t>(0);
                    uint64_t index = 0;
                    for (uint64_t row = 0; row != 2; ++row) {
                        for (uint64_t col = 0; col != 4; ++col) {
                            Assert::AreEqual(data.get()[index], vec[index]);
                            ++index;
                        }
                    }
                }
            }
            {
                uint64_t n = 3 * 5 * 3;
                std::shared_ptr<uint8_t> data1{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data1.get()[i] = i;
                std::shared_ptr<uint8_t> data2{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint8_t>(n) + i;
                {
                    //std::ofstream file("testCoreRead_GetEntity2.big", std::ios_base::binary | std::ios_base::out);
                    //writeHeader(file);
                    //std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 1, 2, 3, 4}; //, 6, 8, 1
                    //writeMetaData(file, metaData);
                    //writeData(file, data1, n, 0);
                    //writeData(file, data2, n, 1);
					big::BigCoreWrite big("testCoreRead_GetEntity2.big", 3, 5, 3); // default data order
					big.addEntity(data1, 0, big::DataTypes::UINT8_T);
					big.addEntity(data2, 1, big::DataTypes::UINT8_T);
                }
                {
                    big::BigCoreRead big("testCoreRead_GetEntity2.big");
                    auto vec = big.getEntity<uint8_t>(0);
                    for (uint64_t index = 0; index != vec.size(); ++index) {
                        Assert::AreEqual(data1.get()[index], vec[index]);
                    }
                    auto vec2 = big.getEntity<uint8_t>(1);
                    for (uint64_t index = 0; index != vec2.size(); ++index) {
                        Assert::AreEqual(data2.get()[index], vec2[index]);
                    }
                }
                {
                    big::BigCoreRead big("testCoreRead_GetEntity2.big", false, 0);
                    auto vec = big.getEntity<uint8_t>(0);
                    for (uint64_t index = 0; index != vec.size(); ++index) {
                        Assert::AreEqual(data1.get()[index], vec[index]);
                    }
                    auto vec2 = big.getEntity<uint8_t>(1);
                    for (uint64_t index = 0; index != vec2.size(); ++index) {
                        Assert::AreEqual(data2.get()[index], vec2[index]);
                    }
                }
            }
            {

				//this test is not change for datatype in data chunk, but is OK because throw exception, if dataorder is not 1 2 3 4
                std::vector<big::ChunkIds> dataOrder = { big::ChunkIds::IMAGE_HEIGHT, big::ChunkIds::IMAGE_WIDTH, big::ChunkIds::NUMBER_OF_PLANES, big::ChunkIds::NUMBER_OF_IMAGES };
                std::vector<big::DataTypes> dataType = { big::DataTypes::UINT16_T };
                uint16_t n = 5 * 3 * 2;
                std::shared_ptr<uint16_t> data1{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data1.get()[i] = 10 * i;
                std::shared_ptr<uint16_t> data2{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint16_t>(n) + i;
                std::shared_ptr<uint16_t> data3{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data3.get()[i] = 2 * static_cast<uint16_t>(n) + i;
                {
                    std::ofstream file("testCoreRead_GetEntity3.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32 };// , 1, 2, 3, 4, 6, 8, 1 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataOrder) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    metaData = { 6, 8 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataType) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);
                    writeData(file, data3, n, 2);
                }
                {
					//catch exception, which is throw if dataorder is not correct
					try { 
						big::BigCoreRead big("testCoreRead_GetEntity3.big");
						for (uint64_t i = 0; i != 3; ++i) {
							auto vec = big.getEntity<uint16_t>(i);
							auto data = data1;
							if (i == 1) data = data2;
							else if (i == 2) data = data3;
							for (uint64_t index = 0; index != vec.size(); ++index) {
								Assert::AreEqual(data.get()[index], vec[index]);
							}
						}
					}
					catch (const char * str)
					{
						Assert::AreEqual(str, "This data order is not supported!");
					}
                }
                {
					//catch exception, which is throw if dataorder is not correct
					try {
						big::BigCoreRead big("testCoreRead_GetEntity3.big", false, 0);
						for (uint64_t i = 0; i != 3; ++i) {
							auto vec = big.getEntity<uint16_t>(i);
							auto data = data1;
							if (i == 1) data = data2;
							else if (i == 2) data = data3;
							for (uint64_t index = 0; index != vec.size(); ++index) {
								Assert::AreEqual(data.get()[index], vec[index]);
							}
						}
					}
					catch (const char * str)
					{
						Assert::AreEqual(str, "This data order is not supported!");
					}
                }
            }
        }

        TEST_METHOD(BigCoreRead_GetImage)
        {
            {
                uint64_t n = 2 * 4;
                std::shared_ptr<uint8_t> data{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (int i = 0; i != n; ++i) data.get()[i] = i;
                {
                    //std::ofstream file("testCoreRead_GetImage1.big", std::ios_base::binary | std::ios_base::out);
                    //writeHeader(file);
                    //std::vector<uint64_t> metaData{ 1, 8, 1, 2, 8, 2, 3, 8, 4, 4, 8, 1, 5, 32, 1, 2, 3, 4}; //, 6, 8, 1 
                    //writeMetaData(file, metaData);
                    //writeData(file, data, n, 0);
					big::BigCoreWrite big("testCoreRead_GetImage1.big", 2, 4, 1); // default data order
					big.addEntity(data, 0, big::DataTypes::UINT8_T);
                }
                {
                    big::BigCoreRead big("testCoreRead_GetImage1.big");
                    auto vec = big.getImage<uint8_t>(0);
                    for (uint64_t index = 0; index != vec.size(); ++index) {
                        Assert::AreEqual(data.get()[index], vec[index]);
                    }
                }
                {
                    big::BigCoreRead big("testCoreRead_GetImage1.big", false, 0);
                    auto vec = big.getImage<uint8_t>(0);
                    for (uint64_t index = 0; index != vec.size(); ++index) {
                        Assert::AreEqual(data.get()[index], vec[index]);
                    }
                }
            }
            {
                uint64_t n = 3 * 5 * 3;
                std::shared_ptr<uint8_t> data1{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data1.get()[i] = i;
                std::shared_ptr<uint8_t> data2{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
                for (uint8_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint8_t>(n) + i;
                {
                    //std::ofstream file("testCoreRead_GetImage2.big", std::ios_base::binary | std::ios_base::out);
                    //writeHeader(file);
                    //std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32, 1, 2, 3, 4 };//, 6, 8, 1
                    //writeMetaData(file, metaData);
                    //writeData(file, data1, n, 0);
                    //writeData(file, data2, n, 1);
					big::BigCoreWrite big("testCoreRead_GetImage2.big", 3, 5, 3); // default data order
					big.addEntity(data1, 0, big::DataTypes::UINT8_T);
					big.addEntity(data2, 1, big::DataTypes::UINT8_T);
                }
                {
                    big::BigCoreRead big("testCoreRead_GetImage2.big");
                    auto vec = big.getImage<uint8_t>(0);
                    for (uint64_t index = 0; index != vec.size(); ++index) {
                        Assert::AreEqual(data1.get()[index], vec[index]);
                    }
                    auto vec2 = big.getImage<uint8_t>(1);
                    for (uint64_t index = 0; index != vec2.size(); ++index) {
                        Assert::AreEqual(data2.get()[index], vec2[index]);
                    }
                }
                {
                    big::BigCoreRead big("testCoreRead_GetImage2.big", false, 0);
                    auto vec = big.getImage<uint8_t>(0);
                    for (uint64_t index = 0; index != vec.size(); ++index) {
                        Assert::AreEqual(data1.get()[index], vec[index]);
                    }
                    auto vec2 = big.getImage<uint8_t>(1);
                    for (uint64_t index = 0; index != vec2.size(); ++index) {
                        Assert::AreEqual(data2.get()[index], vec2[index]);
                    }
                }
            }
            {
				//this test is not change for datatype in data chunk, but is OK because throw exception, if dataorder is not 1 2 3 4
                std::vector<big::ChunkIds> dataOrder = { big::ChunkIds::IMAGE_HEIGHT, big::ChunkIds::IMAGE_WIDTH, big::ChunkIds::NUMBER_OF_PLANES, big::ChunkIds::NUMBER_OF_IMAGES };
                std::vector<big::DataTypes> dataType = { big::DataTypes::UINT16_T };
                uint16_t n = 5 * 3 * 2;
                std::shared_ptr<uint16_t> data1{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data1.get()[i] = 10 * i;
                std::shared_ptr<uint16_t> data2{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint16_t>(n) + i;
                std::shared_ptr<uint16_t> data3{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
                for (uint16_t i = 0; i != n; ++i) data3.get()[i] = 2 * static_cast<uint16_t>(n) + i;
                {
                    std::ofstream file("testCoreRead_GetImage3.big", std::ios_base::binary | std::ios_base::out);
                    writeHeader(file);
                    std::vector<uint64_t> metaData{ 1, 8, 2, 2, 8, 3, 3, 8, 5, 4, 8, 3, 5, 32 };// , 1, 2, 3, 4, 6, 8, 1 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataOrder) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    metaData = { 6, 8 };
                    writeMetaData(file, metaData);
                    metaData.clear();
                    for (const auto &d : dataType) metaData.push_back(static_cast<uint64_t>(d));
                    writeMetaData(file, metaData);
                    writeData(file, data1, n, 0);
                    writeData(file, data2, n, 1);
                    writeData(file, data3, n, 2);
                }
                {
					//catch exception, which is throw if dataorder is not correct
					try {
						big::BigCoreRead big("testCoreRead_GetImage3.big");
						auto vec1 = big.getImage<uint16_t>(0);
						auto vec2 = big.getImage<uint16_t>(1);
						std::shared_ptr<uint16_t> data;
						uint64_t index = 0;
						for (uint64_t row = 0; row != 3; ++row) {
							for (uint64_t col = 0; col != 5; ++col) {
								for (uint64_t plane = 0; plane != 3; ++plane) {
									auto vec = &vec1;
									for (uint64_t imageNum = 0; imageNum != 2; ++imageNum) {
										if (index / n == 0) data = data1;
										else if (index / n == 1) data = data2;
										else if (index / n == 2) data = data3;
										Assert::AreEqual(data.get()[index % n], (*vec)[row * 5 * 3 + col * 3 + plane]);
										++index;
										vec = &vec2;
									}
								}
							}
						}
					}
					catch (const char * str)
					{
						Assert::AreEqual(str, "This data order is not supported!");
					}
                }
                {
					//catch exception, which is throw if dataorder is not correct
					try {
						big::BigCoreRead big("testCoreRead_GetImage3.big", false, 0);
						auto vec1 = big.getImage<uint16_t>(0);
						auto vec2 = big.getImage<uint16_t>(1);
						std::shared_ptr<uint16_t> data;
						uint64_t index = 0;
						for (uint64_t row = 0; row != 3; ++row) {
							for (uint64_t col = 0; col != 5; ++col) {
								for (uint64_t plane = 0; plane != 3; ++plane) {
									auto vec = &vec1;
									for (uint64_t imageNum = 0; imageNum != 2; ++imageNum) {
										if (index / n == 0) data = data1;
										else if (index / n == 1) data = data2;
										else if (index / n == 2) data = data3;
										Assert::AreEqual(data.get()[index % n], (*vec)[row * 5 * 3 + col * 3 + plane]);
										++index;
										vec = &vec2;
									}
								}
							}
						}
					}
					catch (const char * str)
					{
						Assert::AreEqual(str, "This data order is not supported!");
					}
                }
            }
        }
        /*TEST_METHOD(BigCoreRead_diffrentDataTypes) //to do
        {
            
            uint16_t n = 5 * 3 * 2;
            std::shared_ptr<uint16_t> data1{ new uint16_t[n], [](uint16_t *p) {delete[] p; } };
            for (uint16_t i = 0; i != n; ++i) data1.get()[i] = i;
         
            std::shared_ptr<uint8_t> data2{ new uint8_t[n], [](uint8_t *p) {delete[] p; } };
            for (uint8_t i = 0; i != n; ++i) data2.get()[i] = static_cast<uint8_t>(n) + i;
            std::shared_ptr<uint32_t> data3{ new uint32_t[n], [](uint32_t *p) {delete[] p; } };
            for (uint32_t i = 0; i != n; ++i) data3.get()[i] = 2 * static_cast<uint32_t>(n) + i;
            std::shared_ptr<uint64_t> data4{ new uint64_t[n], [](uint64_t *p) {delete[] p; } };
            for (uint64_t i = 0; i != n; ++i) data4.get()[i] = 2 * static_cast<uint64_t>(n) + i;
            std::shared_ptr<int8_t> data5{ new int8_t[n], [](int8_t *p) {delete[] p; } };
            for (int8_t i = 0; i != n; ++i) data5.get()[i] = 2 * static_cast<int8_t>(n) + i;
            std::shared_ptr<int16_t> data6{ new int16_t[n], [](int16_t *p) {delete[] p; } };
            for (int16_t i = 0; i != n; ++i) data6.get()[i] = 2 * static_cast<int16_t>(n) + i;
            std::shared_ptr<int32_t> data7{ new int32_t[n], [](int32_t *p) {delete[] p; } };
            for (int32_t i = 0; i != n; ++i) data7.get()[i] = 2 * static_cast<int32_t>(n) + i;
            std::shared_ptr<int64_t> data8{ new int64_t[n], [](int64_t *p) {delete[] p; } };
            for (int64_t i = 0; i != n; ++i) data8.get()[i] = 2 * static_cast<int64_t>(n) + i;
            std::shared_ptr<float> data9{ new float[n], [](float *p) {delete[] p; } };
            for (int i = 0; i != n; ++i) data9.get()[i] = 2 * static_cast<float>(n) + i;
            std::shared_ptr<double> data10{ new double[n], [](double *p) {delete[] p; } };
            for (int i = 0; i != n; ++i) data10.get()[i] = 2 * static_cast<double>(n) + i;
            std::shared_ptr<half> data11{ new half[n], [](half *p) {delete[] p; } };
            for (int i = 0; i != n; ++i) data11.get()[i] = 2 * static_cast<half> (n) + i;
            {
                big::BigCoreWrite big("testCoreRead_DiffrentDataTypes1.big", 2, 3, 5);
                big.addEntity(data1, 0, big::DataTypes::UINT16_T);
                big.addEntity(data2, 1, big::DataTypes::UINT8_T);
                big.addEntity(data3, 2, big::DataTypes::UINT32_T);
                big.addEntity(data4, 3, big::DataTypes::UINT64_T);
                big.addEntity(data5, 4, big::DataTypes::INT8_T);
                big.addEntity(data6, 5, big::DataTypes::INT16_T);
                big.addEntity(data7, 6, big::DataTypes::INT32_T);
                big.addEntity(data8, 7, big::DataTypes::INT64_T);
                big.addEntity(data9, 8, big::DataTypes::FLOAT);
                big.addEntity(data10, 9, big::DataTypes::DOUBLE);
                big.addEntity(data11, 10, big::DataTypes::HALF);
            }
            big::BigCoreRead big("testCoreRead_DiffrentDataTypes1.big", false, 0);
            Assert::AreEqual(2ull, big.getNumberOfImages());
            Assert::AreEqual(3ull, big.getImageHeight());
            Assert::AreEqual(5ull, big.getImageWidth());
            Assert::AreEqual(3ull, big.getNumberOfPlanes());
            const auto &dataOrder = big.getDataOrder();
            for (uint64_t i = 0; i != dataOrder.size(); ++i) {
                Assert::AreEqual(big::defaultDataOrder[i], dataOrder[i]);
            }
            const auto &dataType = big.getDataType();
            for (uint64_t i = 0; i != dataType.size(); ++i) {
                Assert::AreEqual(big::defaultDataType[0], dataType[i]);
            }

            uint64_t index = 0;
            for (uint64_t imageNum = 0; imageNum != 11; ++imageNum) {
                checkAtandGet(big, data[imageNum], imageNum, 2, 3, 5, n);
        }*/

    };
}