#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <iterator>

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

std::vector<std::vector<std::pair<int,int>>> generateOrientations(int len){
	std::vector<std::vector<std::pair<int,int>>> orientations; // [[(x,y),(x,y+1),(x,y+2)]]
	std::vector<std::pair<int,int>> directions{{0,1},{0,-1},{1,0},{-1,0},{1,1},{-1,-1},{1,-1},{-1,1}};
	for(std::pair<int,int> direction: directions) {
		std::vector<std::pair<int,int>> orientation;
		std::pair<int,int> curr{0,0};
		orientation.push_back(curr);
		for(int i{0} ;i<len-1 ;++i) {
			curr = std::make_pair(curr.first+direction.first, curr.second + direction.second);
			orientation.push_back(curr);
		}
		orientations.push_back(orientation);
	}
	return orientations;
}

std::vector<std::vector<std::pair<int,int>>> getXOrientations(){
	std::vector<std::vector<std::pair<int,int>>> orientations{
		{{0,0},{1,1},{2,2},{2,0},{0,2}}, // Ms facing left
		{{2,2},{1,1},{0,0},{0,2},{2,0}}, // Ms facing right
		{{0,0},{1,1},{2,2},{0,2},{2,0}}, // Ms facing up
		{{2,2},{1,1},{0,0},{2,0},{0,2}}, // Ms facing down
	};
	return orientations;
}

bool checkCharMatch(int x, int y, char c, std::vector<std::string>& lines){
	if(x<0 || static_cast<unsigned long>(x)>= lines.size() || y < 0 || static_cast<unsigned long>(y) >= lines[0].size()){
		return false;
	}
	return (lines[static_cast<unsigned long>(x)][static_cast<unsigned long>(y)] == c);
}

bool checkWordMatch(int x, int y, std::vector<std::pair<int,int>>& orientation, std::string& pattern, std::vector<std::string>& lines){
	for(unsigned long i{0} ; i< pattern.size(); ++i){
		//std::cout << pattern[i] << " at (" << x+orientation[i].first << "," << y+orientation[i].second << ") = " << lines[static_cast<unsigned long>(x+orientation[i].first)][static_cast<unsigned long>(y+orientation[i].second)];
		if(!checkCharMatch(x+orientation[i].first,y+orientation[i].second, pattern[i], lines)){
			return false;
		}
	}
	return true;
}

void sol1() {
	std::vector<std::string> lines;
	parseFile("4/input.txt", lines);
	std::string pattern{"XMAS"};
	std::vector<std::vector<std::pair<int,int>>> orientations = generateOrientations(static_cast<int>(pattern.size()));
	int count{0};
	for(int i{0} ; i < static_cast<int>(lines.size()) ; ++i){
		for(int j{0} ; j < static_cast<int>(lines[0].size()) ; ++j){
			for(std::vector<std::pair<int,int>> orientation: orientations){
				if(checkWordMatch(i,j,orientation,pattern,lines)){
					++count;
				}
			}
		}
	}
	std::cout << "count: " << count;
}

void sol2() {
	std::vector<std::string> lines;
	parseFile("4/input.txt", lines);
	std::string pattern{"MASMS"};
	std::vector<std::vector<std::pair<int,int>>> orientations = getXOrientations();
	int count{0};
	for(int i{0} ; i < static_cast<int>(lines.size()) ; ++i){
		for(int j{0} ; j < static_cast<int>(lines[0].size()) ; ++j){
			for(std::vector<std::pair<int,int>> orientation: orientations){
				if(checkWordMatch(i,j,orientation,pattern,lines)){
					++count;
				}
			}
		}
	}
	std::cout << "count: " << count;
}

int main(){
	sol2();
}
