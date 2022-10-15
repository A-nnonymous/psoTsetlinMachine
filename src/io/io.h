#pragma once

#include "csv.h"
template<typename T>
class TsetlinParser{
    public:
       void parse(std::string fname, int ncol);
    private:
};

/*
    "length" represent the amount of data instances in the size of known dtype,
    if set to zero, then allocate full length of memory space and load it all to memory.
*/
template<typename dtype>
void read_binary(std::string filepath, dtype* outside_array_pointer, size_t length)
{
    if(length == 0)
    {
        std::ifstream fastseek(filepath, std::ios::binary | std::ios::ate);
        length = fastseek.tellg();
        fastseek.close();
        std::cout << "Chosed to load whole file, costs " << length << " bytes of memory.\n";
    }
    std::ifstream bin;
    bin.open(filepath, std::ios::in | std::ios::binary);
    
    bin.read((char *)outside_array_pointer, length * sizeof(dtype));

}

template <typename idxtype, typename dtype>
void buffered_output(dtype item, idxtype &buffer_idx, idxtype buffer_size,
                     dtype *buffer, std::ostream &output) {
  if (buffer_idx < buffer_size) {
    buffer[buffer_idx++] = item;
  } else {
    output.write((char *)buffer, buffer_size * sizeof(dtype));
    buffer_idx = 0;
    buffer[buffer_idx++] = item;
  }
}

void parse_huesken_seqs(std::string path, 
                        std::vector<std::vector<int>> &result);


void parse_huesken_scores(std::string path, 
                          std::vector<std::vector<int>> &result);