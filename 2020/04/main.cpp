#include <algorithm>
#include <cassert>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

using ValidatorFunction = bool(*)(std::string &value);

struct StringValidatorStruct
{
	std::string attribute;
	ValidatorFunction func;
};

static const std::vector<std::string> validEyeColorStrs =
{
	"amb",
	"blu",
	"brn",
	"gry",
	"grn",
	"hzl",
	"oth",
};

static bool validateYearGen(std::string &value, int minYear, int maxYear)
{
	if(value.length() != 4)
		return false;
	int year = atoi(value.c_str());
	return (year >= minYear && year <= maxYear);
}

static bool validateByr(std::string &value)
{
	return validateYearGen(value, 1920, 2002);
}

static bool validateIyr(std::string &value)
{
	return validateYearGen(value, 2010, 2020);
}

static bool validateEyr(std::string &value)
{
	return validateYearGen(value, 2020, 2030);
}

static bool validateHgt(std::string &value)
{
	if(value.length() < 4)
		return false;
	std::string substr = value.substr(value.size() - 2);
	int num = atoi(value.c_str());

	return ((substr == "cm" && num >=150 && num <= 193) ||
		(substr == "in" && num >=59 && num <= 76));
}

static bool validateHcl(std::string &value)
{
	if(value.length() != 7)
		return false;
	if(value[0] != '#')
		return false;
	for(int i = 1; i < 7; ++i)
	{
		char c = value[i];
		if(!((c >= '0' && c <= '9') ||
			(c >= 'a' && c <= 'f')))
			return false;
	}
	return true;
}

static bool validateEcl(std::string &value)
{
	for(const std::string &str : validEyeColorStrs)
	{
		if(str == value)
			return true;
	}
	return false;
}

static bool validatePid(std::string &value)
{
	if(value.length() != 9)
		return false;
	for(const char c : value)
	{
		if(!(c >= '0' && c <= '9'))
			return false;
	}
	return true;
}

static bool validateCid(std::string &value)
{
	return true;
}


static const std::vector<StringValidatorStruct> strings =
{
	{"byr", validateByr},
	{"iyr", validateIyr},
	{"eyr", validateEyr},
	{"hgt", validateHgt},
	{"hcl", validateHcl},
	{"ecl", validateEcl},
	{"pid", validatePid},
	{"cid", validateCid},
};


int readValues(const char *filename)
{
	int count = 0;
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return -1;
	}
	std::string s;
	int foundAtts = 0;
	int accepted = 0;
	int legals = 0;
	int valids = 0;

	while(getline(f, s))
	{
		int chIndex = 0;
		while(chIndex < s.length())
		{
			std::string attr;
			std::string valueStr;
			while(s[chIndex] != ':')
				attr += s[chIndex++];
			++chIndex;
			while(chIndex < s.length() && s[chIndex] != ' ')
				valueStr += s[chIndex++];
			++chIndex;
			for(int i = 0; i < strings.size(); ++i)
			{
				if(attr == strings[i].attribute)
				{
					assert(!(foundAtts & (1 << i)));
					foundAtts |= (1 << i);
					int wasValid = strings[i].func(valueStr) ? 1 : 0;
					foundAtts |= (65536 << i) * wasValid;
				}
			}
		}
		if(s.length() == 0)
		{
			int wasAccepted = ((foundAtts & 0x7f) == 0x7f) ? 1 : 0; 
			accepted += wasAccepted;
			int hadValidValues = ((foundAtts & 0x7f007f) == 0x7f007f) ? 1 : 0;
			valids += hadValidValues;
			foundAtts = 0;
		}

	}
	f.close();

	int wasAccepted = ((foundAtts & 0x7f) == 0x7f) ? 1 : 0; 
	accepted += wasAccepted;
	int hadValidValues = ((foundAtts & 0x7f007f) == 0x7f007f) ? 1 : 0;
	valids += hadValidValues;

	printf("Found: %i, valid ones: %i\n", accepted, valids);
	return true;
}

bool testFunc(ValidatorFunction func, std::string s)
{
	return func(s);
}

int main(int argc, char** argv)
{
	assert(testFunc(validateByr, "2000"));
	assert(testFunc(validateByr, "1950"));
	assert(!testFunc(validateByr, "1900"));
	assert(!testFunc(validateByr, "2003"));
	assert(!testFunc(validateByr, "aaaa"));
	assert(!testFunc(validateByr, "20001"));
	assert(!testFunc(validateByr, "aaa"));
	assert(!testFunc(validateByr, "201"));
	assert(!testFunc(validateByr, ""));

	assert(testFunc(validateIyr, "2010"));
	assert(testFunc(validateIyr, "2015"));
	assert(testFunc(validateIyr, "2020"));
	assert(!testFunc(validateIyr, "2005"));
	assert(!testFunc(validateIyr, "2021"));
	assert(!testFunc(validateIyr, "202"));
	assert(!testFunc(validateIyr, "20212"));
	assert(!testFunc(validateIyr, ""));
	assert(!testFunc(validateIyr, "a"));

	assert(testFunc(validateEyr, "2020"));
	assert(testFunc(validateEyr, "2025"));
	assert(testFunc(validateEyr, "2030"));
	assert(!testFunc(validateEyr, "2019"));
	assert(!testFunc(validateEyr, "2031"));
	assert(!testFunc(validateEyr, "202"));
	assert(!testFunc(validateEyr, "20212"));
	assert(!testFunc(validateEyr, ""));
	assert(!testFunc(validateEyr, "a"));

	assert(testFunc(validateHgt, "150cm"));
	assert(testFunc(validateHgt, "160cm"));
	assert(testFunc(validateHgt, "170cm"));
	assert(testFunc(validateHgt, "180cm"));
	assert(testFunc(validateHgt, "190cm"));
	assert(testFunc(validateHgt, "193cm"));
	assert(testFunc(validateHgt, "59in"));
	assert(testFunc(validateHgt, "69in"));
	assert(testFunc(validateHgt, "76in"));
	assert(!testFunc(validateHgt, ""));
	assert(!testFunc(validateHgt, "in"));
	assert(!testFunc(validateHgt, "cm"));
	assert(!testFunc(validateHgt, "5in"));
	assert(!testFunc(validateHgt, "59"));
	assert(!testFunc(validateHgt, "150cn"));
	assert(!testFunc(validateHgt, "150"));
	assert(!testFunc(validateHgt, "150in"));
	assert(!testFunc(validateHgt, "5in"));
	assert(!testFunc(validateHgt, "16cm"));
	assert(!testFunc(validateHgt, "1600cm"));
	assert(!testFunc(validateHgt, "aaaaa"));

	assert(testFunc(validateHcl, "#012345"));
	assert(testFunc(validateHcl, "#6789ab"));
	assert(testFunc(validateHcl, "#abcdef"));
	assert(!testFunc(validateHcl, ""));
	assert(!testFunc(validateHcl, "abcdef"));
	assert(!testFunc(validateHcl, "a#bcdef"));
	assert(!testFunc(validateHcl, "aa#cdef"));
	assert(!testFunc(validateHcl, "aaacdef"));
	assert(!testFunc(validateHcl, "1234567"));
	assert(!testFunc(validateHcl, "#1234567"));
	assert(!testFunc(validateHcl, "#12345"));

	assert(testFunc(validateEcl, "amb"));
	assert(testFunc(validateEcl, "blu"));
	assert(testFunc(validateEcl, "brn"));
	assert(testFunc(validateEcl, "gry"));
	assert(testFunc(validateEcl, "grn"));
	assert(testFunc(validateEcl, "hzl"));
	assert(testFunc(validateEcl, "oth"));
	assert(!testFunc(validateEcl, "aaa"));
	assert(!testFunc(validateEcl, ""));
	assert(!testFunc(validateEcl, "am"));
	assert(!testFunc(validateEcl, "amba"));

	assert(testFunc(validatePid, "000000000"));
	assert(testFunc(validatePid, "999999999"));
	assert(testFunc(validatePid, "123456789"));
	assert(!testFunc(validatePid, ""));
	assert(!testFunc(validatePid, "12345678a"));
	assert(!testFunc(validatePid, "a12345678"));
	assert(!testFunc(validatePid, "12345678"));
	assert(!testFunc(validatePid, "1234567890"));
	
	if(!readValues("data.txt"))
	{
		printf("Failed to load map!");
		return 0;
	};

	return 0;
}