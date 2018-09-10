#pragma once
#ifndef _BIG_CORE_INPUT_H_
#define _BIG_CORE_INPUT_H_

#include "big_core_base.h"

#include <string>

class BigCoreInput : public BigCoreBase
{
public:
    // Empty constructor. Constructs an empty container, with no elements.
    BigCoreInput();

    // Input constructor. Opens file, reads parameters of the container, but does not read data.
    BigCoreInput(const std::string& fileName);

    // Copy constructor forbidden.
    BigCoreInput(const BigCoreInput &) = delete;

    // destructor
    ~BigCoreInput();

    // Opens file, reads parameters of the container, but does not read data.
    // LB: which parameters? RV: see cpp
    void openFile(const std::string& fileName);

    // Sets maximum possible size of memory that can be used and opens file, reads parameters and data to memory, if possible.
    void readFile(const std::string& fileName, uint64_t bytes = DEFAULT_MEMORY_SIZE);

    // If possible, loads all data to memory. Does nothing if data are bigger than memory size. 
    // LB: should it really do nothing if data are bigger?
    // RV: Pokud se to nevejde, konkretni data se budou cist az na vyzadani a prectena data se zaroven ulozi do bufferu pro rychlejsi opakovany pristup.
    // RV: Casem treba pridame nejake funkce pro nacteni tile do bufferu, aby se pak mohlo rychle pristupovat pomoci []
    // RV: Neni potreba nastavovat jmeno souboru (ani to neni zadouci).
	// LB: nepredaval jsem jmeno souboru, ale soubor -- ale ted jak je to navrzene, musim predat jmeno souboru, z ceho by to jinak nacital
    void loadToMemory(const std::string& filename);

    // LB:
    // All the four methods below are much more complicated -- not checking bounds doesn't seem right
    // also even though operator() may call operator [] as [imageNum * row * col * plane * tileNum]
    // BUT index of data cannot be simply index in char* data, it must be adjusted over each image data type
    // for example let's suppose i want to get some data in second image, where image data type of first image is float,
    // and image type of second image is double therefore to compute index of row * column * plane * tile * image, we need
    // to compute it like this:
    //
    // index = imageWidth * imageHeight * numberOfPlanes * numberOfTiles * sizeof(float) + row * column * plane * tile * sizeof(double)
    //
    // and this index then has to be compared against this->dataLength
    //
    // RV:
    // Ad checking bounds - we need maximal speed, user needs to know what he is doing OR they should use analogous method named "at". See e.g. std::vector.
    // Ad index computation - you are absolutely correct. It is complicated. BUT you can exploit outermostEntitiesOffsets.
    // If you want 10th image, you get address of its start by outermostEntitiesOffsets[9] and than you only compute position of the given element.

    // Access element in given image at given row, column, (color) plane and in given tile. Does not check bounds.
    template<typename T>
    const T & operator() (uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0, uint64_t tileNum = 0) const;

    // Access the outermost entity of the container. If uniformDataType() is true, does not check bounds.
    // RV: tady jsem mel chybu, jako uzivatel chci ukazatel na outermost entity s indexem index (napriklad ukazatel na obrazek cislo 3), abych si z neho mohl dale brat hodnoty co potrebuju
    template<typename T>
    const T * operator[] (uint64_t index) const;

    // Access element in given image at given row, column, (color) plane and in given tile. Checks bounds.
    template<typename T>
    const T & at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0, uint64_t tileNum = 0) const;

    // Access the outermost entity of the container. Checks bounds.
    template<typename T>
    const T * at(uint64_t index) const;

    // LB:
    // Returns a direct pointer to the memory array used internally by the container to store its owned elements.
    // this doesn't make sense -- data might have different data type on each image
    // rather add method const T* getImage
    // RV: Opet kvuli maximalni rychlosti, uzivatel musi vedet co dela, ale nekdy to je proste treba. I std::vector ma tuto metodu.
    // RV: Uznavam, ze template nedava moc smysl, radeji vracet ukazatel na void.
	// LB: data jso typu char, proc void*?
    const char* data() const;

    // Returns tile specified by the number of image and the number of tile.
    // Data must be of size at least height x width x #planes x sizeof(T).
    // LB:
    //template<typename T>
    //T* getTile(uint64_t imageNum, uint64_t tileNum = 0);
    // RV: Libore, Tvoje verze by musela alokovat nove pole, kam by ulozila data. A uzivatel by ho pak musel dealokovat.
    // RV: Radeji bych volil verzi, kam uz posles alokovane pole (a musis si ho po sobe i uklidit)
	// LB: v obou pripadech tu pamet potom musi dealokovat, nevidim v tom rozdil
    template<typename T>
    void getTile(T *data, uint64_t imageNum, uint64_t tileNum = 0);


    // Returns tile specified by the number of image and the number of tile.
    // LB:
    //template<typename T>
    //std::vector<T> getTile(uint64_t imageNum, uint64_t tileNum = 0);
    // RV: Libore, ve Tve verzi bych musel vzdy zadavat datovy typ T, takto ho prekladac odvodi sam ze vstupne-vystupniho vektoru, viz big_core_test.cpp
	// LB: v obou pripadech musi znat datovy typ, jaky chce, taky nevidim rozdil, akorat sev tomhle pripade pri volani mene pise :)
    template<typename T>
    void getTile(std::vector<T>& data, uint64_t imageNum, uint64_t tileNum = 0);

    // todo: introduce constant iterators to iterate through tiles from first tile of the first image to last tile of the last image.

protected:

	// LB: jestli tahle metoda ma byt protected, tak je zbytecne, aby byla staticka
    static bool checkHeader(const char* buffer);
    bool readChunk(std::ifstream& file, uint64_t& id, uint64_t& length);
    void checkDataIntegrity(uint64_t& thisData, const uint64_t loadedData);
    bool readData(std::ifstream& file, const uint64_t id, const uint64_t length);

protected:
    // RV: lock mi prijde zbytecny. Pokud je soubor spatne vytvoren, proste to bude fungovat divne, ale stejne ho neopravime.
	// LB: ja to myslel jinak, ve chvili kdy vytvarim instanci tehle tridy, muzu menit parametry,
	// LB: ve chvili, kdy prdavam data z dalsiho souboru (aspon tak jsem pochopil, ze to ma fungovat), uz nemuzu menit parametry a zaroven ty parametry musi odpovidat puvodnim
	// LB: tj. lock = true, viz readData a checkDataIntegrity
    bool lock;
	uint64_t dataPosition;
};

#endif // _BIG_CORE_INPUT_H_
