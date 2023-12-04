
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

struct AnswerStruct
{
	int left;
	int right;
	bool found;
};

bool readValues(const char *filename, std::vector<int> &outValues)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return false;
	}
	std::string s;
	while(getline(f, s))
	{
		outValues.emplace_back(std::stoi(s));
	}
	f.close();
	
	return true;
}

AnswerStruct findSum(const std::vector<int> &values, int sumToFind, int leftIndex)
{
	int rightIndex = int(values.size()) - 1;
	while(leftIndex < rightIndex)
	{
		int left = values[leftIndex];
		int right = values[rightIndex];
		int sum = left + right;
		if(sum < sumToFind)
		{
			++leftIndex;
		}
		else if(sum > sumToFind)
		{
			--rightIndex;
		}
		else
		{
			// found our answer:
			return AnswerStruct{ left, right, true };
		}
		
	}
	return AnswerStruct{0, 0, false};
}

void first(const std::vector<int> &values)
{
	AnswerStruct answer = findSum(values, 2020, 0);
	if(answer.found)
	{
		int mul = answer.left * answer.right;
		printf("Found answer for 2 numbers: %i * %i = %i\n",
			answer.left, answer.right, mul);
	}
	else
	{
		printf("Didn't find 2 numbers with values that are sum of 2020\n");
	}
}

void second(const std::vector<int> &values)
{
	for(int i = 0; i < values.size(); ++i)
	{
		int firstValue = values[i];
		AnswerStruct answer = findSum(values, 2020 - firstValue, i + 1);
		if(answer.found)
		{
			int64_t mul = answer.left * answer.right * values[i];
			printf("Found answer for 3 numbers: %i * %i * %i = %" PRId64 "\n",
				firstValue, answer.left, answer.right, mul);
			return;
		}
	}

	printf("Didn't find 3 numbers with values that are sum of 2020\n");
}

int main(int argc, char** argv)
{
	std::vector<int> values;
	if(!readValues("data.txt", values))
	{
		return 0;
	}

	if(values.size() < 2)
	{
		printf("Data should have at least 2 numbers!\n");
		return 0;
	}

	int leftIndex = 0;
	int rightIndex = int(values.size()) - 1;

	std::sort(values.begin(), values.end());

	first(values);
	second(values);
	return 0;
}