#pragma once

#include "tapetools/tape.h"

#include <memory>
#include <vector>

namespace tapetools {
    class TapeSort {
    public:
        TapeSort(const Tape* input_tape);

        void sort();

    private:
        std::shared_ptr<Tape> input_tape_;
    };
}