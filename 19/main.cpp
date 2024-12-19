#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <vector>

template <class T>
T fromString(std::string& word) {
	if constexpr (std::is_same<T, int>::value) {
		return T(stoi(word));
	} else if constexpr (std::is_same<T, long long>::value) {
		return T(stoll(word));
	} else if constexpr (std::is_same<T, double>::value) {
		return T(stod(word));
	} else {
		word.erase(remove_if(word.begin(), word.end(), isspace), word.end());
		return T(word);
	}
}

template <class T>
std::vector<T> split(const std::string& str, const std::string delim = " ") {
	std::vector<T> ans;
	unsigned long start{};
	unsigned long end{};
	while ( (start = str.find_first_not_of(delim, end)) != std::string::npos ) {
		end = str.find(delim, start);
		std::string word{str.substr(start, end-start)};
		ans.push_back(fromString<T>(word));
	}
	return ans;
}

void parseFile(std::string filePath, std::vector<std::string>& towels, std::vector<std::string>& patterns){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string line{};
	std::getline(inFile, line);
	towels = split<std::string>(line,",");
	while (std::getline(inFile, line)) {
		if(line.empty()) continue;
		patterns.push_back(line);
	}
	inFile.close();
}

bool isPossible(const std::vector<std::string>& towels, std::string_view pattern, size_t pos = 0){
	if(pos == pattern.size()) return true;
	for(std::string towel: towels) {
		if(towel == pattern.substr(pos,towel.size())) {
			if (isPossible(towels, pattern, pos + towel.size())) return true;
		}
	}
	return false;
}

long long howManyPossible(const std::vector<std::string>& towels, const std::string_view pattern, std::unordered_map<size_t,long long>& posMap, const size_t pos = 0){
	if(pos == pattern.size()) return 1;
	if(posMap.count(pos) > 0) return posMap[pos];
	long long ans{0};
	for(std::string towel: towels) {
		if(towel == pattern.substr(pos,towel.size())) {
			ans += howManyPossible(towels, pattern, posMap, pos + towel.size());
		}
	}
	posMap[pos] = ans;
	return ans;
}

std::vector<std::string> pruneTowels(const std::vector<std::string>& towels, const std::string_view pattern){
	std::vector<std::string> ans{};
	for(std::string s : towels){
		bool possible{true};
		if(pattern.find(s) == std::string::npos) possible = false;
		if(possible) ans.push_back(s);
	}
	return ans;
}

void sol1(){
	std::vector<std::string> towels{};
	std::vector<std::string> patterns{};
	parseFile("19/input.txt",towels,patterns);
	int possible{0};
	for(std::string pattern: patterns){
		if(isPossible(towels, pattern)){
			++possible;
		}
	}
	std::cout << "possible: " << possible;
}

void sol2(){
	std::vector<std::string> towels{};
	std::vector<std::string> patterns{};
	parseFile("19/input.txt",towels,patterns);
	long long possible{0};
	for(std::string pattern: patterns){
		std::vector<std::string> pruned{pruneTowels(towels,pattern)};
		std::unordered_map<size_t,long long> posMap;
		possible += howManyPossible(pruned, pattern, posMap);
	}
	std::cout << "possible: " << possible;
}

int main() {
	sol2();
}