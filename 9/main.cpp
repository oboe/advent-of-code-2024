#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>


void parseFile(std::string filePath, std::vector<std::string>& lines){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string x{};
	while (std::getline(inFile, x)) {
		lines.push_back(x);
	}
	inFile.close();
}

std::vector<int> uncompact(std::string& compacted){
	std::vector<int> ans;
	bool isFile{true};
	int currFileId{0};
	for(char c : compacted){
		const int val{c - '0'};
		for(int i{0}; i < val ; ++i){
			ans.push_back(isFile ? currFileId : -1);
		}
		if(isFile){
			 ++currFileId;
			 isFile = false;
		} else {
			isFile = true;
		}
	}	
	return ans;
}

std::vector<int> compress(std::vector<int>& uncompacted){
	unsigned long emptyIndex{0};
	unsigned long moveIndex{uncompacted.size()-1};
	while(emptyIndex < moveIndex){
		while(uncompacted[emptyIndex] != -1 && emptyIndex < uncompacted.size() && emptyIndex < moveIndex){
			++emptyIndex;
		}
		if(uncompacted[moveIndex] != -1){
			std::swap(uncompacted[moveIndex], uncompacted[emptyIndex]);
		}
		--moveIndex;
	}
	return uncompacted;
}

std::vector<int> wholeFileCompress(std::vector<int>& uncompacted){
	int currFileId{*std::max_element(std::begin(uncompacted), std::end(uncompacted))};
	while(currFileId >= 0){
		auto startIt{std::find(uncompacted.begin(), uncompacted.end(), currFileId)};
		const unsigned long startPos{static_cast<unsigned long>(std::distance(uncompacted.begin(),startIt))};
		const unsigned long endPos{(uncompacted.size() - 1 - static_cast<unsigned long>(std::distance(uncompacted.rbegin(),std::find(uncompacted.rbegin(), uncompacted.rend(), currFileId))))};
		const unsigned long neededSpace{endPos - startPos + 1};

		unsigned long currPos{};
		unsigned long currLen{0};
		for(unsigned long i{0}; i < startPos ; ++i){
			if(uncompacted[i] == -1){
				++currLen;
			} else {
				currLen = 0;
			}
			if(currLen >= neededSpace){
				currPos = i;
				break;
			}
		}
		if (currLen >= neededSpace) {
			std::swap_ranges(startIt, startIt + static_cast<long>(neededSpace), uncompacted.begin() + static_cast<long>(currPos - currLen + 1));
		}
		--currFileId;
	}
	return uncompacted;
}

long long checksum(std::vector<int>& str){
	long long ans{0};
	for(unsigned long i = 0 ; i < str.size(); ++i){
		if (str[i] != -1){
			ans += static_cast<long long>(str[i]) * static_cast<long long>(i);
		}
	}
	return ans;
}

void sol1(){
	std::vector<std::string> lines;
	parseFile("9/input.txt", lines);
	std::string compacted{lines[0]};
	std::vector<int> uncompacted{uncompact(compacted)};
	std::vector<int> compressed{compress(uncompacted)};
	std::cout << "checksum: " << checksum(compressed);
}

void sol2(){
	std::vector<std::string> lines;
	parseFile("9/input.txt", lines);
	std::string compacted{lines[0]};
	std::vector<int> uncompacted{uncompact(compacted)};
	std::vector<int> compressed{wholeFileCompress(uncompacted)};
	std::cout << "checksum: " << checksum(compressed);
}

int main() {
	sol2();
}