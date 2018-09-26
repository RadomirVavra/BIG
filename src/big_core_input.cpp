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

	// this vector must be empty before loading any data!
	this->dataType.clear();

	uint64_t id, length;
	while (!file.eof())
	{
		if (!this->readChunk(file, id, length) && !this->readData(file, id, length))
		{
			throw "Error while reading file!";
		}
	}

	this->setPermutation();
	this->fillEntities();
}

void BigCoreInput::readFile(const std::string& filename, uint64_t bytes)
{
    // RV:
    // 1. volat metodu setMemorySize(bytes)
    // 2. volat metodu openFile(filename)
    // 3. volat metodu loadToMemory()

	this->setMemorySize(bytes);
	this->openFile(filename);
	this->loadToMemory();
}

void BigCoreInput::loadToMemory()
{
    // RV:
    // 1. overit, ze se data vejdou do pameti, else return
    // 2. alokovat prislusne velkou pamet
    // 3. posunout se v souboru na misto, kde zacinaji data (metoda openFile ho ulozila do promenne)
    // 4. nacist data

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
	// TODO: this must be adjusted according to data order. Would only work for image being outermost entity
	// part this->data[this->outermostEntitiesOffsets[imageNum]] is alright, as it points directly to the first
	// element of the image. The rest is wrong.
	return static_cast<const T>(this->data[this->outermostEntitiesOffsets[imageNum] + row * column * plane * tileNum * this->getImageType(this->dataType[dataTypeIndex])]);
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

	int64_t offset = this->outermostEntitiesOffsets[imageNum];

	uint64_t dataTypeIndex = this->isUniformDataType() ? 0 : imageNum;
	if (this->isInMemory())
	{
		return static_cast<const T>(this->data[offset + row * column * plane * tileNum * this->getImageType(this->dataType[dataTypeIndex])]);
	}

	this->file.seekg(this->dataPosition + offset);
	uint64_t size = this->outermostEntitiesOffsets[imageNum + 1] - this->outermostEntitiesOffsets[imageNum];
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

	uint64_t offset = this->outermostEntitiesOffsets[index];

	if (this->isInMemory())
	{
		return static_cast<const T*>(this->data + offset);
	}

	this->file.seekg(this->dataPosition + offset);
	uint64_t size = this->dataLength - offset <= this->memorySize ? this->dataLength - offset : this->memorySize;
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

	// first do some preparation
	uint64_t iterators[] = { 0, 0, 0 };
	uint64_t stoppers[3];
	size_t index = 0;
	for (size_t i = 0; i < 5; i++)
	{
		if (this->dataOrder[i] != 1 && this->dataOrder[i] != 2)
		{
			stoppers[index] = this->permutation[i];
			index++;
		}
	}

	uint64_t imageStart = this->outermostEntitiesOffsets[imageNum];
	for (iterators[0]; iterators[0] < stoppers[0] iterators[0]++)
	{
		for (iterators[1]; iterators[1] < stoppers[1]; iterators[1]++])
		{
			for (iterators[2]; iterators[2] < stoppers[2]; iterators[2]++)
			{
				// TODO
			}
		}
	}

	return tileData;
}

template<typename T>
void BigCoreInput::getTile(std::vector<T>& data, uint64_t imageNum, uint64_t tileNum)
{
	if (imageNum >= this->getNumberOfImages)
		throw "Number of images out of bound!";

	if (tileNum >= this->numberOfTiles)
		throw "Number of tiles out of bound!";

	size_t dataSize = this->imageWidth * this->imageHeight * this->numberOfPlanes;
	// std::vector<T> tileData = std::vector<T>();

    // RV: takto ne, opet "slozita" metoda, viz vyse
	//size_t ptr = 0;
	//if (!this->isUniformDataType)
	//{
	//	for (size_t i = 0; i < imageNum; i++)
	//	{
	//		ptr += this->imageSize(this->dataType[i]);
	//	}
	//}
	//else
	//	ptr = this->imageSize(this->dataType[0]) * imageNum;

	//ptr += dataSize * tileNum;

	//std::copy(this->data + ptr, this->data + ptr + dataSize, tileData);
	return tileData;
}

void BigCoreInput::fillEntities()
{
	uint64_t difference = 1;

	// this computes difference between starts of images. For example it images were inner most entities, 
	// their first elements would be aligned in memory therefore we always need to count outtermostEntities like this
	// otherise we might miss some elements
	for (size_t i = 4; i >= 0; --i)
	{
		if (this->dataOrder[i] == 1) break;

		difference *= this->permutation[i];
	}

	uint64_t offset = this->outermostEntitiesOffsets[0];
	for (size_t i = 0; i < this->numberOfImages; i++)
	{
		offset += difference;
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
		// works for both uniform and nonuniform data types
		for (size_t i = 0; i < length / CHUNK_LENGTH; i++)
		{
			file.read((char*)&dt, CHUNK_LENGTH);
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
