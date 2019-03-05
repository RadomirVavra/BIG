#pragma once

#include <fstream>
#include <memory>
#include "CppUnitTest.h"

#include "../../include/big_consts.hpp"

namespace Microsoft {
    namespace VisualStudio {
        namespace CppUnitTestFramework
        {
            template<> inline std::wstring ToString<uint16_t>(const uint16_t& t) { RETURN_WIDE_STRING(t); }
            template<> inline std::wstring ToString<int64_t>(const int64_t& t) { RETURN_WIDE_STRING(t); }
            template<> inline std::wstring ToString<big::ChunkIds>(const big::ChunkIds& t) { RETURN_WIDE_STRING(static_cast<uint64_t>(t)); }
            template<> inline std::wstring ToString<big::DataTypes>(const big::DataTypes& t) { RETURN_WIDE_STRING(static_cast<uint64_t>(t)); }
        }
    }
}


namespace big_test
{
    void checkHeader(std::ifstream &file);

    void checkMetaData(std::ifstream &file, const std::vector<uint64_t> &metaData);

    template <typename T>
    void checkData(std::ifstream &file, std::shared_ptr<T> data, uint64_t n, uint64_t index);

    void writeHeader(std::ofstream &file);

    void writeMetaData(std::ofstream &file, const std::vector<uint64_t> &metaData);

    template <typename T>
    void writeData(std::ofstream &file, std::shared_ptr<T> data, uint64_t n, uint64_t index);

}

