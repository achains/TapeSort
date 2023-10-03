#pragma once

#include <cstddef>

namespace tapetools {
    class Tape {
    public:
        virtual int readBlock(size_t block_size) = 0;
    };
}