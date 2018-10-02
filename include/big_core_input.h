#pragma once
#ifndef _BIG_CORE_INPUT_H_
#define _BIG_CORE_INPUT_H_

#include "big_core_base.h"

#include <string>

class BigCoreInput : public BigCoreBase
{
public:

    // Empty constructor. Constructs an empty container, with no elements.
    BigCoreInput() {}

    // Input constructor. Opens file, reads parameters of the container, but does not read data.
    BigCoreInput(const std::string& fileName);

    // Copy constructor forbidden.
    BigCoreInput(const BigCoreInput &) = delete;

    // Destructor.
    ~BigCoreInput();

    // Assignment operator forbidden.
    BigCoreInput &operator=(const BigCoreInput &) = delete;

    // Opens file, reads parameters of the container, but does not read data.
    void openFile(const std::string& fileName);

    // Opens file, reads parameters and data to memory, if possible.
    void readFile(const std::string& fileName);

    // Sets maximum possible size of memory that can be used and opens file, reads parameters and data to memory, if possible.
    void readFile(const std::string& fileName, uint64_t bytes);

    // Loads all data to memory, if possible. Does nothing if data are bigger than memory size. 
    void loadToMemory();

    // Returns a direct pointer to the memory array used internally by the container to store its owned elements.
    const char* data() const { return _data; }

    // Access element in a given image at given row, column and (color) plane. Does not check bounds.
    template<typename T>
    const T & operator() (uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0) const;

    // Access the outermost entity of the container. Does not check bounds.
    template<typename T>
    const T * operator[] (uint64_t index) const;

    // Access element in a given image at given row, column and (color) plane. Checks bounds.
    template<typename T>
    const T & at(uint64_t imageNum, uint64_t row, uint64_t col, uint64_t plane = 0) const;

    // Access the outermost entity of the container. Checks bounds.
    template<typename T>
    const T * at(uint64_t index) const;

    // Returns an image specified by its number.
    // Data must be of size at least height x width x #planes x sizeof(T).
    template<typename T>
    void getImage(T *data, uint64_t imageNum);

    // Returns an image specified by its number.
    template<typename T>
    void getImage(std::vector<T>& data, uint64_t imageNum);

protected:

    // Reads chunk's id and its length.
    std::ifstream& readChunk(std::ifstream& file, CoreChunkIds& id, uint64_t& length);

    // Reads chunk's data, e.g., numberOfImages, imageHeight, etc. Does not read the data itself, just remembers its position.
    std::ifstream& readData(std::ifstream& file, CoreChunkIds id, uint64_t length);

protected:

    uint64_t dataPosition = 0;          // position of the data array in a file
    std::ifstream file;                 // attached file
};

#endif // _BIG_CORE_INPUT_H_
