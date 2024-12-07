#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>

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

long long calculatePossibleAndReturn(std::vector<long long>& values, long long ans, bool enableConcat = false){
	std::unordered_set<long long> previouslyPossible;
	for(long long val: values){
		if(previouslyPossible.size() == 0){
			previouslyPossible.insert(val);
			continue;
		}
		std::unordered_set<long long> newlyPossible;
		for(long long lastVal: previouslyPossible){
			newlyPossible.insert(lastVal*val);
			newlyPossible.insert(lastVal+val);
			if (enableConcat) newlyPossible.insert(stoll(std::to_string(lastVal) + std::to_string(val)));
		}
		previouslyPossible = newlyPossible;
	}
	for(long long i : previouslyPossible){
		if(i == ans){
			return ans;
		}
	}
	return 0;
}

void sol1(){
	std::vector<std::string> lines;
	parseFile("7/input.txt", lines);
	long long count{0};
	for(std::string line: lines){
		std::vector<std::string> ansAndValues{split<std::string>(line, ":")};
		const long long ans{stoll(ansAndValues[0])};
		std::vector<long long> values{split<long long>(ansAndValues[1])};
		count += static_cast<long long>(calculatePossibleAndReturn(values, ans));
	}
	std::cout << "count: " << count;
}

void sol2(){
	std::vector<std::string> lines;
	parseFile("7/input.txt", lines);
	long long count{0};
	for(std::string line: lines){
		std::vector<std::string> ansAndValues{split<std::string>(line, ":")};
		const long long ans{stoll(ansAndValues[0])};
		std::vector<long long> values{split<long long>(ansAndValues[1])};
		count += static_cast<long long>(calculatePossibleAndReturn(values, ans, true));
	}
	std::cout << "count: " << count;
}

int main() {
	sol2();
}