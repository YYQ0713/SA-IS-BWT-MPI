#include "SAIS.h"
#include "SuffixArray.h"
#include <omp.h>
//TO DO
bool SAIS::addString(std::string inputString) {
    if (numStrings + 1 > 1)
        return false;
    SuffixArray::addString(inputString);
	return true;
}

//TO DO
bool SAIS::addStringFromFile(std::string fileName)
{
    if (numStrings + 1 > 1)
        return false;
    SuffixArray::addStringFromFile(fileName);
	return true;
}

void SAIS::makeSuffixArray()
{
    
    std::vector<int> Bucket = fillBucket(*string, maxAlphabetSize);
    
    SAIS_SA(*string, *suffixArray, Bucket);

    SuffixArray::suffixArray2BWT(*string, *suffixArray);

}

bool SAIS::isLMSChar(unsigned int offset, const std::vector<bool>& typemap) {
    if (offset == 0)
        return false;
    if (!typemap[offset] && typemap[offset - 1])
        return true;
    
    return false;
}

std::vector<bool> SAIS::buildTypeMap(const std::vector<int>& string) {
    
    std::vector<bool> tmap(string.size() + 1, false);

    *tmap.rbegin() = false;
    if (string.size() == 0) {
        return tmap;
    }

    *(tmap.rbegin() + 1) = true;  
     
    for (int i = tmap.size() - 3; i >= 0; i--) {

        if (string[i] > string[i + 1])
            tmap[i] = true;
        else if (string[i] == string[i + 1] && tmap[i + 1])
            tmap[i] = true;
        else
            tmap[i] = false;
    }


    return tmap;
    
}

std::vector<int> SAIS::fillBucket(std::vector<int>& string, int alphabetSize) {
    int temp[alphabetSize] = {0};
    #pragma omp parallel for reduction(+ : temp) num_threads(16)
    for (int i = 0; i < string.size(); i++) {
        temp[string[i]]++;
    }

    std::vector<int> Bucket(temp, temp + alphabetSize);
    return Bucket;
}

std::vector<int> SAIS::findBucketHeads(std::vector<int>& Bucket) {
    unsigned int offset = 1;
    unsigned int length = Bucket.size();
    std::vector<int> result(length);

    for (auto i = 0; i < Bucket.size(); i++) {
        result[i] = offset;
        offset += Bucket[i];
    }

    return result;
}

std::vector<int> SAIS::findBucketTails(std::vector<int>& Bucket) {
    unsigned int offset = 1;
    unsigned int length = Bucket.size();
    std::vector<int> result(length);

    for (auto i = 0; i < Bucket.size(); i++) {
        offset += Bucket[i];
        result[i] = offset - 1;
    }

    return result;
}

bool SAIS::isLMSSubstringsEqual(const std::vector<int>& string, 
                                const std::vector<bool>& typemap, 
                                unsigned int offset1, 
                                unsigned int offset2) {
    if (offset1 == string.size() || offset2 == string.size())
        return false;
    int i = 0;
    while (true) {
        bool firstIsLMS = isLMSChar(i + offset1, typemap);
        bool secondIsLMS = isLMSChar(i + offset2, typemap);

        if (i > 0 && firstIsLMS && secondIsLMS)
            return true;
        
        if (firstIsLMS != secondIsLMS)
            return false;
        
        if (string[i + offset1] != string[i + offset2])
            return false;
        
        i += 1;
    }
}

std::vector<int> SAIS::guessLMSSort(const std::vector<int>& string, std::vector<int> &Bucket, const std::vector<bool> &typemap) {
    std::vector<int> guessSuffixArray(string.size() + 1, -1);
    auto BucketTails = findBucketTails(Bucket);

    for (int i = 0; i < string.size(); i++) {
        if (isLMSChar(i, typemap)) {
            unsigned int BucketIndex = string[i];
            guessSuffixArray[BucketTails[BucketIndex]] = i;
            BucketTails[BucketIndex] -= 1;
        }
    }

    guessSuffixArray[0] = string.size();
    return guessSuffixArray;
}

void SAIS::inducedSortL(const std::vector<int>& string,
                        std::vector<int>& guessSuffixArray, 
                        std::vector<int>& Bucket, 
                        const std::vector<bool>& typemap) {

    auto BucketHeads = findBucketHeads(Bucket);
    for (auto i = 0U; i < guessSuffixArray.size(); i++) {
        if (guessSuffixArray[i] == -1)
            continue;
        
        int j = guessSuffixArray[i] - 1;
        if (j < 0)
            continue;
        
        if (!typemap[j])
            continue;

        unsigned int BucketIndex = string[j];
        guessSuffixArray[BucketHeads[BucketIndex]] = j;
        BucketHeads[BucketIndex] += 1;
    }
}

void SAIS::inducedSortS(const std::vector<int>& string, 
                        std::vector<int>& guessSuffixArray, 
                        std::vector<int>& Bucket, 
                        const std::vector<bool>& typemap) {
    auto BucketTails = findBucketTails(Bucket);

    for (int i = guessSuffixArray.size() - 1; i >= 0; i--) {
        int j = guessSuffixArray[i] - 1;
        if (j < 0)
            continue;
        
        if (typemap[j])
            continue;

        unsigned int BucketIndex = string[j];
        guessSuffixArray[BucketTails[BucketIndex]] = j;
        BucketTails[BucketIndex] -= 1;
    }
}

int SAIS::summariseSuffixArray(const std::vector<int>& string, 
                               std::vector<int>& guessSuffixArray, 
                               const std::vector<bool>& typemap,
							   std::vector<int>& SummaryString,
							   std::vector<int>& SummaryOffset) {
    std::vector<int> LMSNames(string.size() + 1, -1);
    int currentName = 0;

    LMSNames[guessSuffixArray[0]] = currentName;
    int lastLMSSuffixOffset = guessSuffixArray[0];

    for (int i = 1; i < guessSuffixArray.size(); i++) {
        int suffixOffset = guessSuffixArray[i];

        if (!isLMSChar(suffixOffset, typemap))
            continue;
        
        if (!isLMSSubstringsEqual(string, typemap, lastLMSSuffixOffset, suffixOffset))
            currentName += 1;
        
        lastLMSSuffixOffset = suffixOffset;
        LMSNames[suffixOffset] = currentName;
    }

    for (int i = 0; i < LMSNames.size(); i++) {
        if (LMSNames[i] != -1) {
            SummaryOffset.push_back(i);
            SummaryString.push_back(LMSNames[i]);
        }
    }

    return currentName + 1;
}

std::vector<int> SAIS::makeSummarySuffixArray(std::vector<int>& summaryString, int summaryAlphabetSize) {

	//Every character only appears once
	//Use bucket sort to make suffix array
	if (summaryAlphabetSize == summaryString.size())
	{
        
		std::vector<int> summarySuffixArray(summaryString.size() + 1);
		summarySuffixArray[0] = summaryString.size();

		for (int i = 0; i < summaryString.size(); i++)
		{
			summarySuffixArray[(int)summaryString[i] + 1] = i;
		}
		return summarySuffixArray;
	}
	//Recursion to create suffix array
	else {
        
		std::vector<int> bucket = fillBucket(summaryString, summaryAlphabetSize);
        std::vector<int> target;
		SAIS_SA(summaryString, target, bucket);
        return target;
	}
}

std::vector<int> SAIS::LMSsort(const std::vector<int>& string, std::vector<int>& Bucket, const std::vector<bool>& typemap, std::vector<int>& sumSuffixArray, std::vector<int>& sumOffsets)
{
	std::vector<int> suffixOffsets(string.size() + 1, -1);
	std::vector<int> BucketTails = findBucketTails(Bucket);

	//Reverse order to add suffixes to respective buckets
	for (int i = sumSuffixArray.size() - 1; i > 1; --i)
	{
		int stringIndex = sumOffsets[sumSuffixArray[i]];
		int bucketIndex = string[stringIndex];
		
		//Place suffix at bucket location
		suffixOffsets[BucketTails[bucketIndex]] = stringIndex;
		BucketTails[bucketIndex] -= 1; //Decrement tail position
	}
	//Empty suffix placement
	suffixOffsets[0] = string.size();
	return suffixOffsets;
}

void SAIS::SAIS_SA(std::vector<int>& string, std::vector<int>& target, std::vector<int>& Bucket)
{
	//Compute whether a character is LType or SType
	std::vector<bool> TypeMap = buildTypeMap(string);

	//Bucket-sort to insert all LMS suffixes
	std::vector<int> guess = guessLMSSort(string, Bucket, TypeMap);

	//Move all other suffixes into guess array by induced sorting
	inducedSortL(string, guess, Bucket, TypeMap);
	inducedSortS(string, guess, Bucket, TypeMap);

	//Create new string to summarize relative order of LMS suffixes 
	std::vector<int> summaryString;
	std::vector<int> summaryOffset;
	int summaryAlphabetSize = summariseSuffixArray(string, guess, TypeMap, summaryString, summaryOffset);

	//Make sorted suffix array of summary string
	std::vector<int> summarySuffixArray = makeSummarySuffixArray(summaryString, summaryAlphabetSize);


	//LMS suffix positions are now known
	std::vector<int> finalSuffixArray = LMSsort(string, Bucket, TypeMap, summarySuffixArray, summaryOffset);
	//Finish suffix array by induced sorting
	inducedSortL(string, finalSuffixArray, Bucket, TypeMap);
	inducedSortS(string, finalSuffixArray, Bucket, TypeMap);

	//Copy into the target suffix array
    target.assign(finalSuffixArray.begin(), finalSuffixArray.end());
}