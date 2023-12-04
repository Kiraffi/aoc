
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

static constexpr int MAXIM_INT = 0x7fffffff;

#include <unordered_map>
#include <unordered_set>

struct Node
{
	std::string parent;
	std::vector<std::string> children;
	bool visited = false;
};




bool readValues(const char *filename)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return false;
	}


	std::unordered_map<std::string, Node> nodes;
	std::unordered_set<std::string> parents;

	std::string s;

	while(getline(f, s))
	{
		std::string parentName = "";
		int ind = 0;
		while(s[ind] != ')')
		{
			parentName += s[ind];
			++ind;
		}
		Node &parentNode = nodes[parentName];
		
		std::string childName = s.substr(ind + 1);
		Node &childNode = nodes[childName];

		childNode.parent = parentName;

		parentNode.children.push_back(childName);
		if(parentNode.parent.empty())
		{
			parents.insert(parentName);
		}
		parents.erase(childName);
	}
	f.close();

	int c = 0;

	// nonrecursive
	std::vector<std::string> nextOnes;
	std::vector<std::string> currentOnes;
	
	for(const std::string &parent : parents)
	{
		nextOnes.push_back(parent);
	}
	int level = 1;
	while(!nextOnes.empty())
	{
		currentOnes = nextOnes;
		nextOnes.clear();
		for(const std::string &name : currentOnes)
		{
			const Node &node = nodes[name];
			for(const std::string &childName : node.children)
			{
				c += level;
				nextOnes.push_back(childName);
			}
		}
		++level;
	}
	printf("count: %i\n", c);

	nextOnes.clear();
	currentOnes.clear();
	level = 0;
	nextOnes.push_back(nodes["YOU"].parent);

	bool found = false;
	while(!nextOnes.empty() && !found)
	{
		currentOnes = nextOnes;
		nextOnes.clear();
		for(const std::string &name : currentOnes)
		{
			Node &node = nodes[name];
			if(!node.visited)
			{
				for(const std::string &childName : nodes[name].children)
				{
					if(childName == "SAN")
						found = true;
					nextOnes.push_back(childName);
				}
				nextOnes.push_back(nodes[name].parent);
				node.visited = true;
			}
		}
		++level;
	}
	if(found)
		printf("Distance: %i\n", level - 1);
	else
		printf("Didn't find SAN from YOU");
	return true;
}


int main(int argc, char** argv)
{
	if(!readValues("data.txt"))
	{
		return 0;
	}

	return 0;
}