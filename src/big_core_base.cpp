#include "../include/big_core_base.h"

BigCoreBase::BigCoreBase()
{}

BigCoreBase::~BigCoreBase()
{
    clear();
}

void BigCoreBase::clear()
{
	delete[] _data;
}

bool BigCoreBase::isInMemory()
{
	if(this->data) return true;
	return false;
}

void BigCoreBase::setMemorySize(uint64_t bytes)
{
	this->memorySize = bytes;
}

size_t BigCoreBase::size()
{
	return this->dataLength;
}

size_t BigCoreBase::sizeInMemory()
{
	return this->dataLength;
}

size_t BigCoreBase::getTypeSize(const uint64_t dataType)
{
	DataTypes dType = static_cast<DataTypes>(dataType);

	if (dType == DataTypes::CHAR || dType == DataTypes::UNSIGNED_CHAR || dType == DataTypes::BOOL)
		return 1;

	if (dType == DataTypes::SHORT || dType == DataTypes::UNSIGNED_SHORT || dType == DataTypes::HALF)
		return 2;

	if (dType == DataTypes::FLOAT || dType == DataTypes::INT || dType == DataTypes::UNSIGNED_INT)
		return 4;

	if (dType == DataTypes::DOUBLE || dType == DataTypes::LONG_LONG || dType == DataTypes::UNSIGNED_LONG_LONG)
		return 8;

	return 0;
}

size_t BigCoreBase::imageSizeRaw()
{
	return this->imageWidth * this->imageHeight * this->numberOfPlanes * this->numberOfTiles;
}

size_t BigCoreBase::imageSize(const uint64_t dataType)
{
	return this->getTypeSize(dataType) * this->imageSizeRaw();	
}

bool BigCoreBase::isUniformDataType()
{
	return this->dataType.size() == 1 ? true : false;
}

void BigCoreBase::setPermutations()
{
	const size_t offset = 2;
	for (size_t i = 0; i < 3; i++)
	{
		uint64_t order = this->dataOrder[offset + i];
		if (order == 3) this->permutations[i] = this->imageWidth;
		else if (order == 4) this->permutations[i] = this->imageHeight;
		else if (order == 5) this->permutations[i] = this->numberOfPlanes;
	}
}
