#include "../include/big_core_base.h"

BigCoreBase::~BigCoreBase()
{
    delete[] _data;
}

void BigCoreBase::clear()
{
    numberOfImages = 0;
    imageHeight = 0;
    imageWidth = 0;
    numberOfPlanes = 1;
    dataOrder = { DataOrderIds::NUMBER_OF_IMAGES, DataOrderIds::IMAGE_HEIGHT, DataOrderIds::IMAGE_WIDTH, DataOrderIds::NUMBER_OF_PLANES };
    dataType = { DataTypes::FLOAT };
    delete[] _data;
    _data = nullptr;
    dataSize = 0;
    memorySize = 0;
    dataPosition = 0;
}

void BigCoreBase::setSupportingStructures()
{
    setDimensions();
    setEntityTypeSizes();
    setSubSizes();
    setDataSize();
    setOffsets();
    setOrderMap();
}

void BigCoreBase::setDimensions()
{
    dimensions.clear();
    for (const auto id : dataOrder) {
        switch (id) {
        case DataOrderIds::NUMBER_OF_IMAGES:
            dimensions.push_back(numberOfImages);
            break;
        case DataOrderIds::IMAGE_HEIGHT:
            dimensions.push_back(imageHeight);
            break;
        case DataOrderIds::IMAGE_WIDTH:
            dimensions.push_back(imageWidth);
            break;
        case DataOrderIds::NUMBER_OF_PLANES:
            dimensions.push_back(numberOfPlanes);
            break;
        }
    }
}

void BigCoreBase::setEntityTypeSizes()
{
    entityTypeSizes.clear();
    if (dataType.size() == 1) {
        entityTypeSizes.resize(dimensions[0], typeSizes[static_cast<uint64_t>(dataType[0])]);
    }
    else {
        for (uint64_t i = 1; i != dataType.size(); ++i) {
            entityTypeSizes.push_back(typeSizes[static_cast<uint64_t>(dataType[i])]);
        }
    }
}

void BigCoreBase::setSubSizes()
{
    uint64_t n = dimensions.size();
    subSizes.resize(n);
    subSizes[n - 1] = 1;
    for (uint64_t i = n - 1; i != 0; --i) {
        subSizes[i - 1] = subSizes[i] * dimensions[i];
    }
}

void BigCoreBase::setDataSize()
{
    dataSize = 0;
    for (const auto ts : entityTypeSizes) {
        dataSize += subSizes[0] * ts;
    }
}

void BigCoreBase::setOffsets()
{
    uint64_t offset = 0;
    for (uint64_t i = 0; i != dimensions[0]; ++i) {
        offsets.push_back(offset);
        offset += subSizes[0] * entityTypeSizes[i];
    }
}

void BigCoreBase::setOrderMap()
{
    orderMap.clear();
    for (uint64_t i = 0; i != dataOrder.size(); ++i) {
        orderMap[dataOrder[i]] = i;
    }
}
