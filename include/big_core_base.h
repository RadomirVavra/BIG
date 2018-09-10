#pragma once // RV: nicemu to nevadi a naprosta vetsina prekladacu to podporuje https://en.wikipedia.org/wiki/Pragma_once
             // LB: ja vim, ale je zbytecne mit tohle a jeste include guarda
#ifndef _BIG_CORE_BASE_H_
#define _BIG_CORE_BASE_H_

#include <cstdint>
#include <array>
#include <vector>

// length of chunk
const uint64_t CHUNK_LENGTH = 8;

// big magic number
const std::array<char, CHUNK_LENGTH> MAGIC = { 0x42, 0x49, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00 };

// RV: proc jsi prepsal uint64_t na size_t? Rozhodne to musi byt alespon 64 bitu, 32 bitu nestaci a vzhledem k definici ostatnich veci to zas nemuze byt 128 bitu, takze jedine uint64_t.
const uint64_t DEFAULT_MEMORY_SIZE = 1024ull * 1024ull * 1024ull;

// core chunk ids
enum class CoreChunkIds : uint64_t
{
    NUMBER_OF_IMAGES = 1,
    NUMBER_OF_TILES = 2,
    IMAGE_HEIGHT = 3,
    IMAGE_WIDTH = 4,
    NUMBER_OF_PLANES = 5,
    DATA_ORDER = 6,
    DATA_TYPE = 7,
    DATA = 8,
};

// data type ids
// RV: v big.pdf je jasne definovane, ze je to 8 bitu, protoze tam muze byt dlouhy vektor a usetrime trochu mista. Vice nez 255 datovych typu urcite nebude treba specifikovat.
// LB: kvuli zarovnani dat v souboru na 8 bytu mi to prijde lepsi, nemusi se pak reit zarovni, proste to zarovnane bude, 
// LB: 7 bajtu za obrazek navic si myslim nevadi (pro 1000 obrazku, je to zhruba 7MB, ale 1000 obrazku se do pameti nejspis ani nebude vejit)
enum class DataTypes : uint8_t 
{
    HALF = 1,
    FLOAT = 2,
    DOUBLE = 3,
    CHAR = 4,
    UNSIGNED_CHAR = 5,
    SHORT = 6,
    UNSIGNED_SHORT = 7,
    INT = 8,
    UNSIGNED_INT = 9,
    LONG_LONG = 10,
    UNSIGNED_LONG_LONG = 11,
    BOOL = 12,
};

class BigCoreBase
{
protected:
    // RV: presunul jsem nahoru, je to takto prehlednejsi, kdyz konstruktory a destruktory jsou nahore.
    BigCoreBase();

public:
    ~BigCoreBase();

    // getters
    // todo: provide comments of individual methods
    uint64_t getNumberOfImages();
    uint64_t getNumberOfTiles();
    uint64_t getImageHeight();
    uint64_t getImageWidth();
    uint64_t getNumberOfPlanes();

    // LB: returning references to protected variables should only be allowed from protected methods
    // RV: jsou const, takze za normalnich okolnosti nehrozi prepsani z vnejsku. Jelikoz zejmena dataType muze byt dlouhy vektor, je rychlejsi vratit referenci.
    const std::vector<uint64_t>& getDataOrder();
    const std::vector<uint8_t>& getDataType();

    // Checks whether the container is empty. Returns true until memory is allocated.
    // LB: simply naming it empty() is just super confusing
    // RV: vzorem mi byl std::vector a ostatni kontejnery (viz http://www.cplusplus.com/reference/vector/vector/), ale mas pravdu, ze takto je to asi lepsi
    bool isEmpty();

    // Frees memory. In case of output class, enables to change dimensions, data order or data types.
    void clear();

    // Returns true if all data are in memory.
    bool isInMemory();

    // Sets maximum possible size of memory that can be used. Does not free memory if already allocated.
    // RV: size_t -> uint64_t viz vyse. const jsem smazal, protoze je to u beznych parametru (tedy ne pointer nebo reference) zbytecne a nepouziva se to.
    void setMemorySize(uint64_t bytes = DEFAULT_MEMORY_SIZE);

    // Returns total size of the data.
    uint64_t size();

    // Returns size of the used memory.
    uint64_t sizeInMemory();

    // returns size of givne data type
    // RV: jmeno teto metody mi nejak nesedi s tim, co ma delat.
    size_t getTypeSize(const uint64_t dataType);

    // returns width * height * planes * tiles no matter data type
    // RV: je potreba pracovat spise s jednotlivymi dlazdicemi (tiles)
    // RV: metoda se mi zda zbytecna, vypocet muze byt soucasti imageSize
    size_t imageSizeRaw();

    // returns memory size of one image
    size_t imageSize(const uint64_t dataType);

    // Returns true if all data have the same data type.
    bool isUniformDataType();

protected:
    uint64_t numberOfImages = 0;
    uint64_t numberOfTiles = 1;
    uint64_t imageHeight = 0;
    uint64_t imageWidth = 0;
    uint64_t numberOfPlanes = 1;
    std::vector<uint64_t> dataOrder = { 1, 2, 3, 4, 5 };
    std::vector<uint8_t> dataType = { 2 };
    char *data = nullptr;

    uint64_t dataLength = 0;
    uint64_t memorySize = DEFAULT_MEMORY_SIZE;

	std::vector<uint64_t> outermostEntitiesOffsets = { 0 };
};

#endif // _BIG_CORE_BASE_H_
