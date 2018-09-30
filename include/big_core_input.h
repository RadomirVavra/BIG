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
    void openFile(const std::string& fileName);

    // Sets maximum possible size of memory that can be used and opens file, reads parameters and data to memory, if possible.
    void readFile(const std::string& fileName, uint64_t bytes = DEFAULT_MEMORY_SIZE);

    // If possible, loads all data to memory. Does nothing if data are bigger than memory size. 
    void loadToMemory();

    // Access element in given image at given row, column, (color) plane and in given tile. Does not check bounds.
    template<typename T>
    const T & operator() (uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0, uint64_t tileNum = 0) const;

    // Access the outermost entity of the container. If uniformDataType() is true, does not check bounds.
    template<typename T>
    const T * operator[] (uint64_t index) const;

    // Access element in given image at given row, column, (color) plane and in given tile. Checks bounds.
    template<typename T>
    const T & at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0, uint64_t tileNum = 0) const;

    // Access the outermost entity of the container. Checks bounds.
    template<typename T>
    const T * at(uint64_t index) const;

    // Returns a direct pointer to the memory array used internally by the container to store its owned elements.
    const char* data() const;

    // Returns tile specified by the number of image and the number of tile.
    // Data must be of size at least height x width x #planes x sizeof(T).
    template<typename T>
    void getTile(T *data, uint64_t imageNum, uint64_t tileNum = 0);


    // Returns tile specified by the number of image and the number of tile.
    template<typename T>
    void getTile(std::vector<T>& data, uint64_t imageNum, uint64_t tileNum = 0);

    // todo: introduce constant iterators to iterate through tiles from first tile of the first image to last tile of the last image.

protected:

	void fillEntities();
    bool checkHeader(const char* buffer);
    bool readChunk(std::ifstream& file, uint64_t& id, uint64_t& length);
    bool readData(std::ifstream& file, const uint64_t id, const uint64_t length);

protected:
	uint64_t dataPosition;
	std::ifstream file;
};

#endif // _BIG_CORE_INPUT_H_
