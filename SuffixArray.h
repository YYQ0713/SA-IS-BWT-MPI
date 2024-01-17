#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <deque>

class SuffixArray
{   
public:
    SuffixArray();
    virtual ~SuffixArray();
    //virtual void makeSuffixArray() = 0;
    virtual bool addStringFromFile(std::string);
    virtual bool addString(std::string);
    virtual bool addChar(char *inputSring, size_t size);
    void suffixArray2BWT(const std::vector<int>& string, const std::vector<int>& suffixArray);

    void printSuffixArray();
    void printBWT();
    std::vector<char> dataBWT(); 

protected:
    const int maxAlphabetSize = 256;
    int numStrings;
    std::vector<int> *suffixArray;
    std::vector<int> *string;
    std::vector<char> BWT;

    template <typename T>
    void printVector(std::vector<T> v);

    template <typename T>
    void printSubstringVector(std::vector<T> v, int offset, int length);
};