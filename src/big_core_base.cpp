#include "../include/big_core_base.h"

// RV: general question. Proc vsude pouzivas "this->..."? Osobne mi to prijde mene prehledne a nepouzival bych to. Ale predpokladam, ze pro Tebe to bude naopak. Nebo je jiny duvod?
// LB: jsem zvykly z pythonu, kde je to povinnost (tam je self.<neco>), navic je potom naprosto zrejme, jestli se vola metoda dane tridy, nebo jeho potomka a nebo jestli se vola neco externiho

// RV: general comment. Pri pouziti struktury "if (condition) statement" bud davej vse na jeden radek nebo prosim pouzivej { }. Pokud je to na vice radku bez slozenych zavorek, delam pak nekdy chybu, ze pripisu dalsi statement da dalsi radek a zapomenu pridat { }. Diky
// RV: general comment. Vsechny komentare, ktere pravdepodobne nebudou soucasti finalni verze a jsou to spise otazky nebo odpovedi na me komentare prosim oznacuj inicialy LB, podobne jako to delam ja. Takoveto komentare mohou byt cesky bez diakritiky.
// RV: general comment. V cpp souborech dodrzuj prosim pokud mozno stejne poradi metod jako v hlavickovem souboru. Lepe se pak hleda a kontroluje, co je naimplementovano.

BigCoreBase::BigCoreBase()
{}

BigCoreBase::~BigCoreBase()
{
    if (this->data) delete[] this->data;
}

uint64_t BigCoreBase::getNumberOfImages()
{
	return this->numberOfImages;
}

uint64_t BigCoreBase::getNumberOfTiles()
{
	return this->numberOfTiles;
}

uint64_t BigCoreBase::getImageHeight()
{
	return this->imageHeight;
}

uint64_t BigCoreBase::getImageWidth()
{
	return this->imageWidth;
}

uint64_t BigCoreBase::getNumberOfPlanes()
{
	return this->numberOfPlanes;
}

const std::vector<uint64_t>& BigCoreBase::getDataOrder()
{
    return dataOrder;
}

const std::vector<uint64_t>& BigCoreBase::getDataType()
{
    return dataType;
}

bool BigCoreBase::isEmpty()
{
	return this->dataLength == 0 ? true : false;
}

void BigCoreBase::clear()
{
	if (this->data) delete[] this->data;
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

