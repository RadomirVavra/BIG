#pragma once
#ifndef _BIG_CORE_WRITE_H_
#define _BIG_CORE_WRITE_H_

#include "big_core_base.hpp"

#include <fstream>
#include <memory>

namespace big
{
    class BigCoreWrite : public BigCoreBase
    {
    public:

        // Constructor. Constructs an empty container, sets dimensions of the container, data order and data type(s).
        // Opens the given file and writes meta-data, but does not write the data itself yet.
        BigCoreWrite(const std::string& fileName, uint64_t numberOfImages, uint64_t imageHeight, uint64_t imageWidth, uint64_t numberOfPlanes = 1, const std::vector<ChunkIds>& dataOrder = defaultDataOrder, const std::vector<DataTypes>& dataType = defaultDataType);

        // Copy constructor forbidden.
        BigCoreWrite(const BigCoreWrite &) = delete;

        // Move constructor.
        BigCoreWrite(BigCoreWrite &&other) = default;

        // Destructor. 
        ~BigCoreWrite();

        // Copy-assignment operator forbidden.
        BigCoreWrite &operator=(const BigCoreWrite &) = delete;

        // Move-assignment operator.
        BigCoreWrite &operator=(BigCoreWrite &&rhs) = default;

    public:

        // Adds an entity to the container and writes it to the file.
        template<typename T>
        void addEntity(std::shared_ptr<T> data, uint64_t index);

    protected:

        // Write chunk's id, length and data.
        bool writeChunk(ChunkIds id);

    protected:

        std::ofstream file;             // attached file
    };

}

#endif // _BIG_CORE_WRITE_H_ 