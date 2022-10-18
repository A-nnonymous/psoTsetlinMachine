#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include "TsetlinMachine.h"

#define COMPLETED true
#define FAILED false

std::vector<int> siRNA2SIG(std::string raw_string);

bool DNA2SIG(std::string raw_inputpath, std::string raw_outputpath, int buffer_size, bool cleave_zero);

void parse_huesken_seqs(std::string path, 
                        std::vector<std::vector<int>> &result);

void parse_huesken_scores(std::string path, 
                          std::vector<std::vector<int>> &result);


bool write_csv_row(std::vector<float> data, std::ofstream *output);

template<typename dtype>
bool write_csv(dtype *data, int row, int column, std::string filepath)
{ 
    std::ofstream output;
    output.open(filepath + ".csv", std::ios::out);
    std::cout << "Output file stream opening success. " << std::endl;
    for(int j = 0; j < row; j++)
    {
        for (int i = 0; i < column - 1; i++)
        {
            output << data[j * column + i] << ",";
        }
        output << data[j*column + column-1]<<"\n";
    }
    output.close();
    return COMPLETED;
    
}
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

template<typename dtype>
bool write_binary(dtype *data, int length, std::string filepath)
{
    std::cout.precision(2);
    std::stringstream sstream;
    std::ofstream output;
    std::string suffix;
    sstream << typeid(dtype).name(); sstream >> suffix; suffix = "." + suffix;
    std::string fullpath = filepath + suffix;
    
    std::cout << "=======================" << "\n";
    output.open(fullpath, std::ios::out |std::ios::binary);
    std::cout << "Output file stream for " << fullpath << " is successfully opened. " << "\n";
    std::cout << "Estimated storage usage: " << std::fixed << length * sizeof(dtype) / (double)(1024 * 1024) << "MB" << "\n";

    std::cout << "Start to output......" << "\n";
    output.write((char*)data, length * sizeof(dtype));
    output.close();
    std::cout << "Output for " << fullpath << " is completed. " << "\n";
    std::cout << "\n";
    return true;
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

void
modelOutput (TsetlinMachine tm,
             std::vector<TsetlinMachine::Clause> bestPositiveClauses,
             std::vector<TsetlinMachine::Clause> bestNegativeClauses,
             float precision,
             std::string outputpath);


