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

int charToInt(char c){
	if (c == '.') return -1;
	return static_cast<int>(c - '0');
}

int calcTrailheadScore(std::vector<std::string>& map, int x, int y, int last = -1, bool countDistinctPaths = false){
	if(x < 0 || static_cast<size_t>(x) >= map.size() || y < 0 || static_cast<size_t>(y) >= map[0].size()) {
		return 0;
	}
	if (last + 1 != charToInt(map[static_cast<unsigned long>(x)][static_cast<unsigned long>(y)])) {
		return 0;
	}
	if (last == 8){
		if (!countDistinctPaths) map[static_cast<unsigned long>(x)][static_cast<unsigned long>(y)] = '.';
		return 1;
	}
	int score{0};
	score += calcTrailheadScore(map, x+1, y, last + 1, countDistinctPaths);
	score += calcTrailheadScore(map, x-1, y, last + 1, countDistinctPaths);
	score += calcTrailheadScore(map, x, y+1, last + 1, countDistinctPaths);
	score += calcTrailheadScore(map, x, y-1, last + 1, countDistinctPaths);
	return score;
}

void sol1(){
	std::vector<std::string> lines;
	parseFile("10/input.txt", lines);
	int totalScore{0};
	for(int x{0}; x < static_cast<int>(lines.size()); ++x){
		for(int y{0}; y < static_cast<int>(lines[0].size()) ; ++y){
			std::vector<std::string> map{lines};
			totalScore += calcTrailheadScore(map, x, y);			
		}
	}
	std::cout << "total score: "<< totalScore;
}

void sol2(){
	std::vector<std::string> lines;
	parseFile("10/input.txt", lines);
	int totalScore{0};
	for(int x{0}; x < static_cast<int>(lines.size()); ++x){
		for(int y{0}; y < static_cast<int>(lines[0].size()) ; ++y){
			totalScore += calcTrailheadScore(lines, x, y, -1, true);			
		}
	}
	std::cout << "total score: "<< totalScore;
}

int main() {
	sol2();
}