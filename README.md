![CI Tests](https://github.com/achains/TapeSort/actions/workflows/unit-tests.yml/badge.svg)

# TapeSort 
Library provides tape device interface and sort algorithm.
## Tape Emulation
This library provides `Tape` interface for common I/O-operations with tapes 
(e.g. move head, read/write, rewind etc.)
There is also `FileTape` class that emulates tape device with operations on binary file. 
### Sort algorithm
Tape size `N` is considered to be much bigger than available on machine RAM size `M`.

To bypass this restriction one can use external sort algorithm consists in sorting at once `K` temporary tape blocks of size `B` each,
so that $K = \frac{M}{B}$ and then performing k-way merge.

## Sort executable
```bash
./tape-sort input_tape input_config output_tape output_config [buffer_size] [block_size] 
```

## Building from source
```bash
cmake -B build [-DBUILD_TESTS=ON]
cmake --build build 
```

## LICENSE
Apache License, Version 2.0

