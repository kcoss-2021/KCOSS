# KCOSS

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

```bash
Example: ./kcoss -k 32 -i "../test_file/test_data.fa" -t 48 -m 360 -o out_file -n 3000000000 -d 163969647
```

Parameters:
- -k \<value\>			k-mer length (k from 1 to 64)
- -i \<string\>			input file path
- -t \<value\>			number of threads
- -m \<value\>			sizes of block (m >1)
- -o \<string\>			output file path
- -n \<value\>			sizes of bloomfilter(Number of k-mer species)
- -d \<value\>			sizes of hash table(Number of k-mer species,Non-single occurrence)
