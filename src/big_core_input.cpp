#include <algorithm>
#include <fstream>
#include <cstring>

#include "../include/big_core_base.h"
#include "../include/big_core_input.h"

BigCoreInput::BigCoreInput()
	:
	dataPosition(0)
{}

BigCoreInput::BigCoreInput(const std::string& filename)
{
	this->openFile(filename);
}

BigCoreInput::~BigCoreInput()
{
	if (this->file)
		this->file.close();
}

void BigCoreInput::openFile(const std::string& filename)
{
	this->dataType.clear();

	this->file.open(filename, std::ios_base::binary);
	
	std::ifstream file(filename);
	if (!file)
	{
		throw "Unable to open file for reading!";
	}

	char buffer[CHUNK_LENGTH];

	file.read(buffer, CHUNK_LENGTH);
	if (!this->checkHeader(buffer))
	{
		throw "File format is not BIG format!";
	}

	uint64_t id, length;
	while (!file.eof())
	{
		if (!this->readChunk(file, id, length) && !this->readData(file, id, length))
		{
			throw "Error while reading file!";
		}
	}

	this->fillEntities();
	this->setPermutations();
}

void BigCoreInput::readFile(const std::string& filename, uint64_t bytes)
{
	this->setMemorySize(bytes);
	this->openFile(filename);
	this->loadToMemory();
}

void BigCoreInput::loadToMemory()
{
	this->clear();
    if (this->dataLength <= this->memorySize)
    {
        // allocate new memory and load data to it
		file.seekg(this->dataPosition);
        this->data = new char[this->dataLength];
        file.read(this->data, this->dataLength);
    }
}

template<typename T>
const T& BigCoreInput::operator()(uint64_t imageNum, uint64_t row, uint64_t column, uint64_t plane, uint64_t tileNum) const
{
	uint64_t dataTypeIndex = this->isUniformDataType() ? 0 : imageNum;
	uint64_t dataSize = this->getTypeSize(dataTypeIndex);
	uint64_t tileOffset = this->outermostEntitiesOffsets[imageNum] + tileNum * this->imageHeight * this->imageWidth * this->numberOfPlanes * dataSize;

	uint64_t tileOrder[] = { 0, 0, 0 };
	for(size_t i = 0; i < 3; i++)
	{ 
		uint64_t order = this->dataOrder[i + 2];
		if (order == 3) tileOrder[i] = row;
		else if (order == 4) tileOrder[i] = column;
		else if (order == 5) tileOrder[i] = plane;
	}
	uint64_t inTileOffset = tileOrder[0] * this->permutations[1] * this->permutations[2] + tileOrder[1] * this->permutations[2] + tileOrder[2];

	return static_cast<const T>(this->data[tileOffset + inTileOffset * dataSize]);
}

template<typename T>
const T* BigCoreInput::operator[](size_t index) const
{
    return static_cast<const T*>(this->data + this->outermostEntitiesOffsets[index]);
}

template<typename T>
const T& BigCoreInput::at(uint64_t imageNum, uint64_t row, uint64_t column, uint64_t plane, uint64_t tileNum) const
{
	if (imageNum >= this->numberOfImages)
		throw "Image number out of bound!";

	if (row >= this->imageHeight)
		throw "Row out of bound!";

	if (column >= this->imageWidth)
		throw "Column out of bound!";

	if (plane >= this->numberOfPlanes)
		throw "Number of planes out of bound!";

	if (tileNum >= this->numberOfTiles)
		throw "Tile number out of bound!";

	uint64_t dataTypeIndex = this->isUniformDataType() ? 0 : imageNum;
	uint64_t dataSize = this->getTypeSize(dataTypeIndex);
	uint64_t tileOffset = this->outermostEntitiesOffsets[imageNum] + tileNum * this->imageHeight * this->imageWidth * this->numberOfPlanes * dataSize;

	uint64_t tileOrder[] = { 0, 0, 0 };
	for (size_t i = 0; i < 3; i++)
	{
		uint64_t order = this->dataOrder[i + 2];
		if (order == 3) tileOrder[i] = row;
		else if (order == 4) tileOrder[i] = column;
		else if (order == 5) tileOrder[i] = plane;
	}
	uint64_t inTileOffset = tileOrder[0] * this->permutations[1] * this->permutations[2] + tileOrder[1] * this->permutations[2] + tileOrder[2];

	if (this->isInMemory())
	{
		return static_cast<const T>(this->data[tileOffset + inTileOffset * dataSize]);
	}

	this->file.seekg(this->dataPosition + tileOffset + inTileOffset * dataSize);

	// rest of the image
	uint64_t size = this->outermostEntitiesOffsets[imageNum + 1] - this->outermostEntitiesOffsets[imageNum] - tileOffset + inTileOffset * dataSize;
	char* ret = new char[size];
	this->file.read(ret, size);
	return static_cast<const T>(ret);
}

template<typename T>
const T* BigCoreInput::at(uint64_t index) const
{
	if (index >= this->dataLength / sizeof(T))
	{
		throw "Index out of bound!";
	}

	if (this->isInMemory())
	{
		return static_cast<const T*>(this->data + outermostEntitiesOffsets[index]);
	}

	this->file.seekg(this->dataPosition + index);
	uint64_t size = this->dataLength - index <= this->memorySize ? this->dataLength - index : this->memorySize;
	char* ret = new char[size];
	this->file.read(ret, size);
	return static_cast<const T*>(ret);
}

const char* BigCoreInput::data() const
{
	return this->data;
}

template<typename T>
void BigCoreInput::getTile(T* data, uint64_t imageNum, uint64_t tileNum)
{
	if (imageNum >= this->getNumberOfImages)
		throw "Number of images out of bound!";

	if (tileNum >= this->numberOfTiles)
		throw "Number of tiles out of bound!";

	uint64_t index = this->dataType.size() == 1 ? 0 : imageNum;
	size_t dataSize = this->getTypeSize(this->dataType[index]);

	// compute tile offset in data
	uint64_t tileOffset = this->outermostEntitiesOffsets[imageNum] + tileNum * this->getImageWidth * this->imageHeight * numberOfPlanes * dataSize;
	for (size_t i = 0; i < this->permutations[0]; i++)
	{
		uint64_t first_offset = i * this->permutations[1] * this->permutations[2];
		for (size_t j = 0; j < this->permutations[1]; j++)
		{
			uint64_t second_offset = j * this->permutations[2];
			for (size_t k = 0; k < this->permutations[2]; k++)
			{
				data[first_offset + second_offset + k] = static_cast<T>(this->data[tileOffset + (first_offset + second_offset + k) * dataSize]);
			}
		}
	}
}

template<typename T>
void BigCoreInput::getTile(std::vector<T>& data, uint64_t imageNum, uint64_t tileNum)
{
	if (imageNum >= this->getNumberOfImages)
		throw "Number of images out of bound!";

	if (tileNum >= this->numberOfTiles)
		throw "Number of tiles out of bound!";
	
	uint64_t index = this->dataType.size() == 1 ? 0 : imageNum;
	size_t dataSize = this->getTypeSize(this->dataType[index]);

	// compute tile offset in data
	uint64_t tileOffset = this->outermostEntitiesOffsets[imageNum] + tileNum * this->getImageWidth * this->imageHeight * numberOfPlanes * dataSize;
	for (size_t i = 0; i < this>permutations[0]; i++)
	{
		uint64_t first_offset = i * this->permutations[1] * this->permutations[2];
		for (size_t j = 0; j < this->permutations[1]; j++)
		{
			uint64_t second_offset = j * permutations[2];
			for (size_t k = 0; k < this->permutaions[2]; k++)
			{
				data.push_back(static_cast<T>(this->data[tileOffset + (first_offset + second_offset + k]) * dataSize));
			}
		}
	}
}

void BigCoreInput::fillEntities()
{
	uint64_t offset = 0;
	for (uint64_t dType : this->dataType)
	{
		offset += this->imageSize(dType);
		this->outermostEntitiesOffsets.push_back(offset);
	}
}

bool BigCoreInput::checkHeader(const char* buffer)
{
    for (size_t i = 0; i < CHUNK_LENGTH; i++)
        if (buffer[i] != MAGIC[i])
            return false;

    return true;
}

bool BigCoreInput::readChunk(std::ifstream& file, uint64_t& id, uint64_t& length)
{
    file.read((char*)&id, CHUNK_LENGTH);
    file.read((char*)&id, CHUNK_LENGTH);
    return !file.fail();
}

bool BigCoreInput::readData(std::ifstream &file, const uint64_t id, const uint64_t length)
{
    CoreChunkIds cid = static_cast<CoreChunkIds>(id);

	if (cid == CoreChunkIds::NUMBER_OF_IMAGES)
	{
		file.read((char*)&this->numberOfImages, length);
	}
    else if (cid == CoreChunkIds::NUMBER_OF_TILES)
    {
        file.read((char*)&this->numberOfTiles, length);
    }
    else if (cid == CoreChunkIds::IMAGE_WIDTH)
    {
        file.read((char*)&this->imageWidth, length);
    }
    else if (cid == CoreChunkIds::IMAGE_HEIGHT)
    {
        file.read((char*)&this->imageHeight, length);
    }
    else if (cid == CoreChunkIds::NUMBER_OF_PLANES)
    {
        file.read((char*)&this->numberOfPlanes, length);
    }
    else if (cid == CoreChunkIds::DATA_ORDER)
    {
        for (int i = 0; i < 5; ++i)
            file.read((char*)&this->dataOrder[i], CHUNK_LENGTH);
    }
    else if (cid == CoreChunkIds::DATA_TYPE)
    {
        uint64_t dt;
		for (size_t i = 0; i < length / CHUNK_LENGTH; i++)
		{
			file.read((char*)&dt, length);
			this->dataType.push_back(dt);
		}
    }
    else if (cid == CoreChunkIds::DATA)
    {
		this->dataPosition = file.tellg();
		this->dataLength = length;
    }
    return !file.fail();
}
