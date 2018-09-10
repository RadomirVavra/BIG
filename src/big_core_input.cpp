#include <algorithm>
#include <fstream>
#include <cstring>

#include "../include/big_core_base.h"
#include "../include/big_core_input.h"

BigCoreInput::BigCoreInput()
	:
	lock(false),
	dataPosition(0)
{}

BigCoreInput::BigCoreInput(const std::string& filename)
{
	this->openFile(filename);
}

BigCoreInput::~BigCoreInput()
{}

void BigCoreInput::openFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios_base::binary);
	
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
	this->lock = true;
}

void BigCoreInput::readFile(const std::string& filename, uint64_t bytes)
{
    // RV:
    // 1. volat metodu setMemorySize(bytes)
    // 2. volat metodu openFile(filename)
    // 3. volat metodu loadToMemory()

	this->setMemorySize(bytes);
	this->openFile(filename);
	this->loadToMemory(filename);
}

void BigCoreInput::loadToMemory(const std::string& filename)
{
    // RV:
    // 1. overit, ze se data vejdou do pameti, else return
    // 2. alokovat prislusne velkou pamet
    // 3. posunout se v souboru na misto, kde zacinaji data (metoda openFile ho ulozila do promenne)
    // 4. nacist data

	std::ifstream file(filename, std::ios_base::binary);
	uint64_t length;
	file.seekg(this->dataPosition - CHUNK_LENGTH);
	file.read((char*)&length, CHUNK_LENGTH);

    if (this->dataLength + length <= this->memorySize)
    {
        // allocate new memory and copy old data to it
        char* data = new char[this->dataLength + length];
        std::copy(this->data, this->data + this->dataLength, data);
		
        // clear old data and assign new ones
        this->clear();
        this->data = data;

        // move pointer past already loaded data and load new ones
        file.read(this->data + this->dataLength, length);
		this->dataLength += length;
		this->outermostEntitiesOffsets.push_back(this->dataLength);
    }
}

// RV: template musi byt v hlavicce
template<typename T>
const T& BigCoreInput::operator(uint64_t imageNum, uint64_t row, uint64_t column, uint64_t plane, uint64_t tileNum) const
{
    // RV: V zasede OK, ale chce to urychlit pouzitim outermostEntitiesOffsets, aby se nemusel napocitavat ten index
    
    size_t index = 0;
	for (size_t i = 0; i < imageNum; i++)
	{
		index += this->imageSize(this->dataType[i]);
	}

	return static_cast<T>(this->data[index + row * column * plane * tileNum * this->getImageType(this->dataType[imageNum])]);
}

template<typename T>
const T* BigCoreInput::operator[](size_t index) const
{
    // RV: tady jsem mel chybu, jako uzivatel chci ukazatel na outermost entity s indexem index (napriklad ukazatel na obrazek cislo 3), abych si z neho mohl dale brat hodnoty co potrebuju
	// LB: tenhle return ale vrati i vsechny nasledujici obrazky
    return static_cast<const T*>(this->data + outermostEntitiesOffsets[index]);
}

template<typename T>
const T& BigCoreInput::at(uint64_t imageNum, uint64_t row, uint64_t column, uint64_t plane, uint64_t tileNum) const
{
    // RV: musi byt ostre znamenko vetsi. Pokud mam napr. 3 obrazky, indexy jsou 0,1,2.
	// LB: musi byt >=, pokud budu v tvem pripade chtit obrazek s indexem 3, tak jsem out of bounds (pokud indexujem od nuly, na coz spoleham)
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

    // RV: TODO: overit, zda jsou data v pameti a pokud ne, tak je nacist 
	// LB: odkud?

	size_t index = 0;
	if (!this->isUniformDataType())
	{
		for (size_t i = 0; i < imageNum; i++)
		{
			index += this->imageSize(this->dataType[i]);
		}
	}
	else
	{
		index = this->imageSize(this->dataType[0]) * imageNum;
	}

	return static_cast<T>(this->data[index + row * column * plane * tileNum * this->getImageType(this->dataType[imageNum])]);
}

template<typename T>
const T* BigCoreInput::at(uint64_t index) const
{
    // RV:
    // viz operator[]
    // tady chci to same, ale s kontrolou, ze jsou data opravdu v pameti a pripadne odpovidajici entitu (napr obrazek nacist do pameti)
	// LB: zase - nacist odkud?

	// following logic only makes sence for uniform data types
	// for non-uniform data type it would be computationally easier to use previous method -- see commented code

	/*
	size_t imageNo = 0;
	size_t it = 0;
	while(imageNo < this->numberOfImages && it + imageSize(this->dataType[imageNo]) < index)
	{
		it += imageSize(this->dataType[imageNo++];
	}

	if(imageNo >= this->numberOfImages && index < this->dataLength)
	std::throw "index == " << index << " out of bound";

	return static_cast<T>(this->data[index]);
	*/

	if (index >= this->dataLength / sizeof(T))
		throw "Index out of bound!";

	return static_cast<const T*>(this->data + outermostEntitiesOffsets[index]);
}

template<typename T>
T* BigCoreInput::getTile(uint64_t imageNum, uint64_t tileNum)
{
	if (imageNum >= this->getNumberOfImages)
		throw "Number of images out of bound!";

	if (tileNum >= this->numberOfTiles)
		throw "Number of tiles out of bound!";

	size_t dataSize = this->imageWidth * this->imageHeight * this->numberOfPlanes;
    // RV: zde je prave ta alokace dat, ktera je trochu problem, protoze uzivatel pak pravdepodobne zapomene uklidit
    // RV: radeji tedy verzi, kde uzivatel uz musi predat alokovane pole, do ktereho se data jen zapisi
	T* tileData = new T[dataSize];

    // RV: tady to bohuzel nebude tak jednoduche. Toto jsem prave myslel tou "slozitou" metodou. Musi se vyzobat jednotlive elementy a poskladat do pole
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

template<typename T>
std::vector<T> BigCoreInput::getTile(uint64_t imageNum, uint64_t tileNum)
{
	if (imageNum >= this->getNumberOfImages)
		throw "Number of images out of bound!";

	if (tileNum >= this->numberOfTiles)
		throw "Number of tiles out of bound!";

	size_t dataSize = this->imageWidth * this->imageHeight * this->numberOfPlanes;
	std::vector<T> tileData = std::vector<T>();

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

void BigCoreInput::checkDataIntegrity(uint64_t& thisData, uint64_t loadedData)
{
    if (!this->lock)
        thisData = loadedData;
    else if (thisData != loadedData)
        throw "Newly loaded data are not consistent with ones already loaded";
}

bool BigCoreInput::readData(std::ifstream &file, const uint64_t id, const uint64_t length)
{
    CoreChunkIds cid = static_cast<CoreChunkIds>(id);

    if (cid == CoreChunkIds::NUMBER_OF_IMAGES)
        file.read((char*)&this->numberOfImages, length);
    else if (cid == CoreChunkIds::NUMBER_OF_TILES)
    {
        uint64_t not;
        file.read((char*) & not, length);

        this->checkDataIntegrity(this->numberOfTiles, not);
    }
    else if (cid == CoreChunkIds::IMAGE_WIDTH)
    {
        uint64_t iw;
        file.read((char*)&iw, length);

        this->checkDataIntegrity(this->imageWidth, iw);
    }
    else if (cid == CoreChunkIds::IMAGE_HEIGHT)
    {
        uint64_t ih;
        file.read((char*)&ih, length);

        this->checkDataIntegrity(this->imageHeight, ih);
    }
    else if (cid == CoreChunkIds::NUMBER_OF_PLANES)
    {
        uint64_t nop;
        file.read((char*)&nop, length);

        this->checkDataIntegrity(this->numberOfPlanes, nop);
    }
    else if (cid == CoreChunkIds::DATA_ORDER)
    {
        // call readVector on following two cases and integrity check for only this one
        for (int i = 0; i < 5; ++i)
            file.read((char*)&this->dataOrder[i], CHUNK_LENGTH);
    }
    else if (cid == CoreChunkIds::DATA_TYPE)
    {
        uint64_t dt;
        file.read((char*)&dt, length);
        this->dataType.push_back(dt);
    }
    else if (cid == CoreChunkIds::DATA)
    {
        // RV: tady radeji jen ulozit pozici v souboru
		this->dataPosition = file.tellg();
    }
    return !file.fail();
}
