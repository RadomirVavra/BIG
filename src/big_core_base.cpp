#include "../include/big_core_base.h"

BigCoreBase::BigCoreBase()
{
    cache.initialize(entitySizes, dataPositions);
}

uint64_t BigCoreBase::getDimension(uint64_t index)
{
    if (dimensions.size() == 0) setSupportingStructures();

    if (index >= dimensions.size())
        throw "Index out of bounds!";

    return dimensions[index];
}

uint64_t BigCoreBase::getEntitySize(uint64_t index)
{
    if (entitySizes.size() == 0) setSupportingStructures();

    if (index >= entitySizes.size())
        throw "Index out of bounds!";

    return entitySizes[index];
}

uint64_t BigCoreBase::getEntityBaseSize()
{
    if (subSizes.size() == 0) setSupportingStructures();
    return subSizes[0];
}

DataTypes BigCoreBase::getEntityDataType(uint64_t index)
{
    if (dataType.size() == 1) return dataType[0];

    if (index + 1 >= dataType.size())
        throw "Index out of bounds!";

    return dataType[index + 1];
}

uint64_t BigCoreBase::getEntityTypeSize(uint64_t index)
{
    if (entityTypeSizes.size() == 0) setSupportingStructures();

    if (index >= entityTypeSizes.size())
        throw "Index out of bounds!";

    return entityTypeSizes[index];

}

uint64_t BigCoreBase::getDataTypeSize(DataTypes dataType)
{
    uint64_t index = static_cast<uint64_t>(dataType);

    if (index >= typeSizes.size())
        throw "Unknown data type!";

    return typeSizes[index];
}

uint64_t BigCoreBase::getNumberOfEntities()
{
    if (dimensions.size() == 0) return 0;
    return dimensions[0];
}

void BigCoreBase::clear()
{
    numberOfImages = 0;
    imageHeight = 0;
    imageWidth = 0;
    numberOfPlanes = 1;
    dataOrder = defaultDataOrder;
    dataType = defaultDataType;

    cache.clear();

    offsets.clear();
    entityTypeSizes.clear();
    entitySizes.clear();
    dimensions.clear();
    orderMap.clear();
    subSizes.clear();
    dataSize = 0;
    dataPositions.clear();
}

void BigCoreBase::setSupportingStructures()
{
    setDimensions();
    setEntityTypeSizes();
    setSubSizes();
    setEntitySizes();
    setDataSize();
    setOffsets();
    setOrderMap();
    dataPositions.resize(dimensions[0], 0);
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

void BigCoreBase::setEntitySizes()
{
    entitySizes.clear();
    for (const auto& ts : entityTypeSizes) {
        entitySizes.push_back(ts * subSizes[0]);
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
