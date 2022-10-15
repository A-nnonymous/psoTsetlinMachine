#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

#define COMPLETED true
#define FAILED false

/*
class CSVRow
{
    public:
        std::string_view operator[](std::size_t index) const
        {
            return std::string_view(&m_line[m_data[index] + 1], m_data[index + 1] -  (m_data[index] + 1));
        }
        std::size_t size() const
        {
            return m_data.size() - 1;
        }
        void readNextRow(std::istream& str)
        {
            std::getline(str, m_line);

            m_data.clear();
            m_data.emplace_back(-1);
            std::string::size_type pos = 0;
            while((pos = m_line.find(',', pos)) != std::string::npos)
            {
                m_data.emplace_back(pos);
                ++pos;
            }
            // This checks for a trailing comma with no data after it.
            pos   = m_line.size();
            m_data.emplace_back(pos);
        }
    private:
        std::string         m_line;         //  line buffer
        std::vector<int>    m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}   
class CSVIterator
{   
    public:
        typedef std::input_iterator_tag     iterator_category;
        typedef CSVRow                      value_type;
        typedef std::size_t                 difference_type;
        typedef CSVRow*                     pointer;
        typedef CSVRow&                     reference;

        CSVIterator(std::istream& str)  :m_str(str.good()?&str:nullptr) { ++(*this); }
        CSVIterator()                   :m_str(nullptr) {}

        // Pre Increment
        CSVIterator& operator++()               {if (m_str) { if (!((*m_str) >> m_row)){m_str = nullptr;}}return *this;}
        // Post increment
        CSVIterator operator++(int)             {CSVIterator    tmp(*this);++(*this);return tmp;}
        CSVRow const& operator*()   const       {return m_row;}
        CSVRow const* operator->()  const       {return &m_row;}
        bool operator==(CSVIterator const& rhs) {return ((this == &rhs) || ((this->m_str == nullptr) && (rhs.m_str == nullptr)));}
        bool operator!=(CSVIterator const& rhs) {return !((*this) == rhs);}
    private:
        std::istream*       m_str;
        CSVRow              m_row;
};

class CSVRange
{
    std::istream&   stream;
    public:
        CSVRange(std::istream& str): stream(str){}
        CSVIterator begin() const {return CSVIterator{stream};}
        CSVIterator end()   const {return CSVIterator{};}
};
*/

bool DNA2SIG(std::string raw_inputpath, std::string raw_outputpath, int buffer_size, bool cleave_zero);

// siRNA specific functions
bool write_csv_row(std::vector<float> data, std::ofstream *output);

std::vector<int> siRNA2SIG(std::string raw_string);

template<typename dtype>
bool write_csv(dtype *data, int row, int column, std::string filepath)
{ 
    std::ofstream output;
    output.open(filepath + ".csv", std::ios::out);
    std::cout << "Output file stream opening success. " << std::endl;
    for(int j = 0; j < row; j++)
    {
        for (int i = 0; i < column; i++)
        {
            output << data[j * row + i] << ",";
        }
        output << "\n";
    }
    output.close();
    return COMPLETED;
    
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