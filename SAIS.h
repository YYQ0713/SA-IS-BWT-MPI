#pragma once
#include "SuffixArray.h"

class SAIS : public SuffixArray
{
public:
    SAIS() {};
    void makeSuffixArray();
	bool addString(std::string);
    bool addStringFromFile(std::string);

private:
    void SAIS_SA(std::vector<int>&, std::vector<int>& target, std::vector<int>&);
    std::vector<bool> buildTypeMap(const std::vector<int>&);
	std::vector<int> fillBucket(std::vector<int>&, int alphabetSize);
    std::vector<int> findBucketHeads(std::vector<int>&);
    std::vector<int> findBucketTails(std::vector<int>&);
    std::vector<int> guessLMSSort(const std::vector<int>& string, std::vector<int> &Bucket, const std::vector<bool> &typemap);
    std::vector<int> LMSsort(const std::vector<int>&, std::vector<int>& bucketSizes, const std::vector<bool>& typemap, std::vector<int>& sumArray, std::vector<int>& sumOffsets);

	void inducedSortL(const std::vector<int>&, std::vector<int>&, std::vector<int>&, const std::vector<bool>&);
	void inducedSortS(const std::vector<int>&, std::vector<int>&, std::vector<int>&, const std::vector<bool>&);

	int summariseSuffixArray(const std::vector<int>& string, 
                             std::vector<int>& guessSuffixArray, 
                             const std::vector<bool>& typemap,
							 std::vector<int>& SummaryString,
							 std::vector<int>& SummaryOffset);
	std::vector<int> makeSummarySuffixArray(std::vector<int>&, int);

    bool isLMSChar(unsigned int offset, const std::vector<bool>&);
    bool isLMSSubstringsEqual(const std::vector<int>&, const std::vector<bool>&, unsigned int, unsigned int);
};