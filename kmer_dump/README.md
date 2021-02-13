# KCOSS_dump

## Requirements

This software has been tested on Ubuntu 20.04.

It requires libcuckoo for building. Check [here](https://github.com/efficient/libcuckoo) for installation guide.

It compiles with Cmake 3.16.3 and G++ 9.3.0.

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

Example: ./kmer_dump 32 360 out_file
