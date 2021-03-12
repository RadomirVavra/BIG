#pragma once
#ifndef _BIG_CORE_WRITE_H_
#define _BIG_CORE_WRITE_H_

#include "big_core_base.hpp"
#include "half.hpp"
#include "pugixml.hpp"

#include <fstream>
#include <memory>
#include <string>


namespace big
{
    class BigCoreWrite : public BigCoreBase
    {
    public:

        // Constructor. Constructs an empty container, sets dimensions of the container, data order and data type(s).
        // Opens the given file and writes meta-data, but does not write the data itself yet.
        BigCoreWrite(const std::string& fileName,  uint64_t imageHeight, uint64_t imageWidth, uint64_t numberOfPlanes = 1, const std::vector<ChunkIds>& dataOrder = defaultDataOrder);

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

        // Adds an entity to the container and writes it to the file, to selected index
        template<typename T>
        void addEntity(std::shared_ptr<T> data, uint64_t index, DataTypes dataType);

        // Adds an entity to the container and writes it to the file, to actual index
        template<typename T>
        void pushEntity(std::shared_ptr<T> data, DataTypes dataType);

        //add XML file to XML chunk
        void writeXML(std::string XMLfile);



    protected:

        // Write chunk's id, length and data.
        bool writeChunk(ChunkIds id);


        //check if file exist
        inline bool file_exist(const std::string&name);


        std::string remove_extension(const std::string& filename);

    protected:

        std::ofstream file;             // attached file
       
    };

}

#endif // _BIG_CORE_WRITE_H_ 