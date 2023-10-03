#pragma once

#include "tape.h"

namespace tapetools {
    class FileTape : public Tape {
    public:
        FileTape();

        int readBlock(size_t block_size) override;

    private:
        const char *tape_path;
    };
}