#include "SuffixArray.h"
#include <omp.h>

SuffixArray::SuffixArray() {
    numStrings = 0;
    suffixArray = new std::vector<int>;
    string = new std::vector<int>;
}

SuffixArray::~SuffixArray() {
    delete suffixArray;
    delete string;
}

/*
 *  Add String to build the Suffix Array
 */
bool SuffixArray::addString(std::string inputSring) {
    for (auto it = inputSring.cbegin(); it != inputSring.cend(); it++) {
        string->push_back(*it);
    }

    numStrings += 1;
    return true;
}

bool SuffixArray::addChar(char *inputSring, size_t size) {
    for (int i = 0; i < size; i++) {
        string->push_back(inputSring[i]);
    }
    return true;
}

bool SuffixArray::addStringFromFile(std::string filename) {
    std::ifstream fin(filename/*, std::ios::binary*/);
    if (!fin) {
        std::cerr << "Could not read file from: " << filename << std::endl;
        return false;
    }

    //Do not skip whitespace characters
    fin.unsetf(std::ios::skipws);

    //get size of file
    fin.seekg(0, std::ios::end);
    std::streampos length = fin.tellg();
    fin.seekg(0, std::ios::beg);

    //reserve the string
    string->reserve(length);

    int i = 0;
    char *temp = new char;
    while (i < length)
    {
        fin.read(temp, sizeof(*temp));
        string->push_back(reinterpret_cast<unsigned char&>(*temp));
        i++;
    }
    delete temp;

    numStrings += 1;
    return true;
}

template <typename T>
void SuffixArray::printVector(std::vector<T> v) 
{
	for (auto i = v.begin(); i != v.end(); i++)
	{
		std::cout << (char)(*i) << ' ';
	}
	std::cout << std::endl;
}


void SuffixArray::printSuffixArray()
{
	std::cout << "Suffix Array : ";
	printVector(*suffixArray);
}

template <typename T>
void SuffixArray::printSubstringVector(std::vector<T> v, int offset, int length) 
{
	if (offset + length <= v.size()) {
		for (auto i = offset; i < offset + length; i++)
		{
			std::cout << (char)v.at(i) << ' ';
		}
		std::cout << std::endl;
	}
	else {
		std::cerr << "Given offset & length for SubVector Print out of range" << std::endl;
		std::cerr << "Offset: " << offset << std::endl;
		std::cerr << "Length: " << length << std::endl;
		std::cerr << "Vector Size: " << v.size() << std::endl;
	}
}

void SuffixArray::suffixArray2BWT(const std::vector<int>& string, const std::vector<int>& suffixArray) {

    int n = suffixArray.size() - 1;
    BWT.resize(n, 0);
    
    //omp_set_num_threads(2);

#pragma omp parallel num_threads(16)
{
    //int size = omp_get_num_threads();
    //std::cout << "num of threads: " << size << std::endl;
    int index;
    #pragma omp for 
    for (int i = 1; i < suffixArray.size(); i++) {
        index = (suffixArray[i] - 1 + n) % n;
        BWT[i - 1] = string[index];
    }
}
    
}

void SuffixArray::printBWT()
{
	std::cout << "BWT : ";
	printVector(BWT);
}

std::vector<char> SuffixArray::dataBWT() {
    return this->BWT;
}