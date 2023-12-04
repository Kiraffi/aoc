
#include <algorithm>
#include <fstream>
#include <math.h>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <vector>
 

bool readValues(const char *filename, std::vector<uint8_t> &outNumbers)
{
	int count = 0;
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return -1;
	}
	std::string s;
	while(getline(f, s))
	{
		for(char c : s)
		{
			if(c >= '0' && c <= '9')
				outNumbers.push_back(c - '0');
		}
	}
	f.close();
	
	return true;
}

void printOutput(const std::vector<uint8_t> &numbers, int offset)
{
	printf("offset: %i, number: ", offset);
	for(int i = 0; i < 8; ++i) //numbers.size(); ++i)
	{
		int off = (i + offset) % numbers.size();
		printf("%i", numbers[off]);
	}
	printf("\n");

}
int64_t minn(int64_t a, int64_t b)
{
	return a < b ? a : b;
}

std::vector<uint8_t> calculateFFTOrig(const std::vector<uint8_t> &readNumbers, int repeatAmount)
{
	std::vector<uint8_t> allNumbers;
	allNumbers.reserve(readNumbers.size() * repeatAmount);
	for(int i = 0; i < repeatAmount; ++i)
		allNumbers.insert(allNumbers.end(), readNumbers.begin(), readNumbers.end());

	std::vector<uint8_t> numbers[2] = { allNumbers, allNumbers };

	int64_t origCount = readNumbers.size();
	int64_t sz = numbers[0].size();

	int indexRead = 0;
	int indexWrite = 1;
	{
		int64_t seq[] = {0, 1, 0, -1};
		for(int i = 0; i < 100; ++i)
		{
			std::vector<uint8_t> &readNumbers = numbers[indexRead];
			std::vector<uint8_t> &writeNumbers = numbers[indexWrite];


			for(int64_t j = 0; j < sz; ++j)
			{
				int64_t sum = 0;
				for(int64_t k = j; k < sz; ++k)
				{
					int64_t s = ((k + 1) / (j + 1)) % 4;
					sum += int64_t(readNumbers[k] * seq[s]);
					if(s == 0) k += j;
					
				}
				sum = sum > 0 ? sum : -sum;
				sum *= repeatAmount;
				writeNumbers[j] = sum % 10;
			}
			indexRead = (indexRead + 1) & 1; 
			indexWrite = (indexWrite + 1) & 1; 
		}
	}
	
	return numbers[indexRead];
}





std::vector<uint8_t> calculateFFT(const std::vector<uint8_t> &readNumbers, int repeatAmount, int64_t upToNumber)
{
	std::vector<uint8_t> allNumbers;
	allNumbers.reserve(readNumbers.size() * repeatAmount);
	for(int i = 0; i < repeatAmount; ++i)
		allNumbers.insert(allNumbers.end(), readNumbers.begin(), readNumbers.end());

	std::vector<uint8_t> numbers[2] = { allNumbers, allNumbers };
 
	int64_t origCount = readNumbers.size();
	int64_t sz = numbers[0].size();

	std::vector<uint64_t> sums(sz, 0);
	std::vector<int64_t> counters;
	counters.reserve(sz);
	int indexRead = 0;
	int indexWrite = 1;
	{
		int64_t seq[] = {0, 1, 0, -1};
		int64_t seq2[] = {-1, -1, 1, 1};
		for(int i = 0; i < 100; ++i)
		{
			std::vector<uint8_t> &readNumbers = numbers[indexRead];
			std::vector<uint8_t> &writeNumbers = numbers[indexWrite];
			memset(sums.data(), 0, sizeof(int64_t) * sz);
			int64_t sum = 0;
			counters.clear();
			counters.push_back(sz - 1);

			while(counters[0] >= upToNumber)
			{
				int64_t j = counters[0];
				int counterSz = counters.size();
				int64_t lastIndex = counterSz + 1;
				lastIndex = (sz) / lastIndex;
				// last numbers up to this point are always just sum of previous,
				// because the multiply by 1 repeats j+1 times.
				while(j >= lastIndex)
				{
					sum += readNumbers[j];
					int64_t tmp = sum;
					for(int k = 1; k < counterSz; ++k)
					{
						int m = (k - 1) % 4;
						m = seq2[m];
						tmp += sums[counters[k]] * m;
						counters[k] -= (k + 1);
					}
					sums[j] = sum;
					tmp = tmp > 0 ? tmp : -tmp;
					writeNumbers[j] = tmp % 10;
					--j;
				}
				if(lastIndex == 0)
					break;
				int64_t nxt = j + (j + 1) * counterSz;
				while(nxt < sz)
				{
					counters.push_back(nxt);
					nxt += j + 1;
				}
				if(counters.size() > origCount * (j / 2))
					break;
				counters[0] = j;
			}
			indexRead = (indexRead + 1) & 1; 
			indexWrite = (indexWrite + 1) & 1;
		}
	}
	
	return numbers[indexRead];
}

int main(int argc, char** argv)
{
	std::vector<uint8_t> readNumbers;
	if(!readValues("data.txt", readNumbers))
	{
		printf("Failed to load map!");
		return 0;
	};

	std::vector<uint8_t> partA = calculateFFTOrig(readNumbers, 1);
	printf("Part a:");
	printOutput(partA, 0);


	//for(int j = 2; j < 9; ++j)
	{
	printf("Part b:");
	int offset = 0;
	for(int i = 0; i < 7; ++i)
		offset = offset * 10 + readNumbers[i];
	std::vector<uint8_t> partB = calculateFFT(readNumbers, 10000, offset);
	//offset = 0;
	printOutput(partB, offset);
	}


	return 0;
}