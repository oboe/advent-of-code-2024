#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>


template <class T>
std::vector<T> split(const std::string& str, const std::string delim = " ") {
	std::vector<T> ans;
	unsigned long start{};
	unsigned long end{};
	while ( (start = str.find_first_not_of(delim, end)) != std::string::npos ) {
		end = str.find(delim, start);
		std::string word{str.substr(start, end-start)};
		if constexpr (std::is_same<T, int>::value) {
			ans.push_back(T(stoi(word)));
		} else if constexpr (std::is_same<T, long long>::value) {
			ans.push_back(T(stoll(word)));
		} else {
			ans.push_back(T(word));
		}
	}
	return ans;
}

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

long long blink(const std::vector<long long>& stones, const int iterations, std::unordered_map<long long, std::unordered_map<int, long long>>& stoneBreakIterationMap);

long long breakStone(const long long stone, const int iterations, std::unordered_map<long long, std::unordered_map<int, long long>>& stoneBreakIterationMap){
	if (stoneBreakIterationMap.count(stone) && stoneBreakIterationMap[stone].count(iterations)){
		return stoneBreakIterationMap[stone][iterations];
	}
	if (iterations == 0){
		constexpr long long ans{1};
		stoneBreakIterationMap[stone][iterations] = ans;
		return ans;
	}
	std::vector<long long> currAns{};
	const std::string digits{std::to_string(stone)};
	if(stone == 0) {
		currAns = {1};
	} else if (digits.size() % 2 == 0) {
		const unsigned long mid{digits.size()/2};
		currAns = {std::stoll(digits.substr(0,mid)), std::stoll(digits.substr(mid,mid))};
	} else {
		currAns = {stone * 2024};
	}
	const long long ans{blink(currAns, iterations -1, stoneBreakIterationMap)};
	stoneBreakIterationMap[stone][iterations] = ans;
	return ans;
}

long long blink(const std::vector<long long>& stones, const int iterations, std::unordered_map<long long, std::unordered_map<int, long long>>& stoneBreakIterationMap){
	long long ans{};
	for(long long stone: stones){
		ans += breakStone(stone, iterations, stoneBreakIterationMap); 
	}
	return ans;
}

void sol1(){
	std::vector<std::string> lines;
	parseFile("11/input.txt", lines);
	std::vector<long long> stones{split<long long>(lines[0])};
	std::unordered_map<long long, std::unordered_map<int, long long>> stoneBreakIterationMap{};
	std::cout << "total stones: " << blink(stones, 25, stoneBreakIterationMap);
}

void sol2(){
	std::vector<std::string> lines;
	parseFile("11/input.txt", lines);
	std::vector<long long> stones{split<long long>(lines[0])};
	std::unordered_map<long long, std::unordered_map<int, long long>> stoneBreakIterationMap{};
	std::cout << "total stones: " << blink(stones, 75, stoneBreakIterationMap);
}

int main() {
	sol2();
}