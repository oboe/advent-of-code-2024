#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>


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

std::unordered_map<char, std::vector<std::pair<unsigned long, unsigned long>>> getAntennaMap(std::vector<std::string>& lines){
	std::unordered_map<char, std::vector<std::pair<unsigned long, unsigned long>>> antennaMap;
	for(unsigned long x{0}; x<lines.size(); ++x){
		for(unsigned long y{0}; y < lines[0].size(); ++y){
			if (lines[x][y] != '.'){
				antennaMap[lines[x][y]].push_back(std::make_pair(x,y));
			}
		}
	}
	return antennaMap;	
}

bool isInBounds(std::pair<unsigned long, unsigned long> location, std::vector<std::string>& lines){
	if(location.first < 0 || location.first >= lines.size() || location.second < 0 || location.second >= lines[0].size()){
		return false;
	}
	return true;
}

void sol1(){
	std::vector<std::string> lines;
	parseFile("8/input.txt", lines);
	std::unordered_map<char, std::vector<std::pair<unsigned long, unsigned long>>> antennaMap{getAntennaMap(lines)};
	std::unordered_set<std::string> antinodes;
	for(auto [frequency, locations] : antennaMap){
		for(std::pair<int, int> locationA: locations) {
			for(std::pair<int, int> locationB: locations) {
				if(locationA.first == locationB.first && locationA.second == locationB.second){
					continue;
				}
				if(locationB.first > locationA.first){
					std::swap(locationA, locationB);
				}
				const int xDiff = locationA.first - locationB.first;
				const int yDiff = locationA.second - locationB.second;
				if (isInBounds(std::make_pair(locationA.first+xDiff,locationA.second+yDiff), lines)){
					antinodes.insert(std::to_string(locationA.first+xDiff)+"-"+ std::to_string(locationA.second+yDiff));
					lines[static_cast<unsigned long>(locationA.first+xDiff)][static_cast<unsigned long>(locationA.second+yDiff)] = '#';
				}
				if (isInBounds(std::make_pair(locationB.first-xDiff,locationB.second-yDiff), lines)){
					antinodes.insert(std::to_string(locationB.first-xDiff)+"-"+ std::to_string(locationB.second-yDiff));
					lines[static_cast<unsigned long>(locationB.first-xDiff)][static_cast<unsigned long>(locationB.second-yDiff)] = '#';
				}
			}
		}
	}
	for(std::string s : lines){
		std::cout << s << "\n";
	}
	std::cout << "antinodes: " <<antinodes.size();
}

void sol2(){
	std::vector<std::string> lines;
	parseFile("8/input.txt", lines);
	std::unordered_map<char, std::vector<std::pair<unsigned long, unsigned long>>> antennaMap{getAntennaMap(lines)};
	std::unordered_set<std::string> antinodes;
	for(auto [frequency, locations] : antennaMap){
		for(std::pair<int, int> locationA: locations) {
			for(std::pair<int, int> locationB: locations) {
				if(locationA.first == locationB.first && locationA.second == locationB.second){
					continue;
				}
				if(locationB.first > locationA.first){
					std::swap(locationA, locationB);
				}
				const int xDiff = locationA.first - locationB.first;
				const int yDiff = locationA.second - locationB.second;
				int xPos = locationA.first;
				int yPos = locationA.second;
				while (isInBounds(std::make_pair(xPos,yPos), lines)){
					antinodes.insert(std::to_string(xPos)+"-"+std::to_string(yPos));
					lines[static_cast<unsigned long>(xPos)][static_cast<unsigned long>(yPos)] = '#';
					xPos += xDiff;
					yPos += yDiff;
				}
				xPos = locationB.first;
				yPos = locationB.second;
				while (isInBounds(std::make_pair(xPos,yPos), lines)){
					antinodes.insert(std::to_string(xPos)+"-"+std::to_string(yPos));
					lines[static_cast<unsigned long>(xPos)][static_cast<unsigned long>(yPos)] = '#';
					xPos -= xDiff;
					yPos -= yDiff;
				}
			}
		}
	}
	for(std::string s : lines){
		std::cout << s << "\n";
	}
	std::cout << "antinodes: " <<antinodes.size();
}

int main() {
	sol2();
}