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

int processMul(std::string mulString){
	std::string l{};
	std::string r{};
	bool commaSeen{false};
	for(unsigned long i = 0 ; i < mulString.size() ; ++i){
		if(i <= 3) {
			continue; // mul(
		}
		if(mulString[i] == ',') {
			commaSeen = true;
			continue;
		}
		if(commaSeen) {
			r += mulString[i];
		} else {
			l += mulString[i];
		}
	}
	return stoi(l) * stoi(r);
}

void sol1() {
	std::vector<std::string> lines;
	parseFile("3/input.txt", lines);
	std::regex mulPattern("mul\\(\\d{1,3},\\d{1,3}\\)");
	int ans{0};
	for(std::string line : lines){
		auto words_begin = std::sregex_iterator(line.begin(), line.end(), mulPattern);
		auto words_end = std::sregex_iterator();
		for(std::sregex_iterator i = words_begin; i != words_end; ++i){
			std::string match = (*i).str();
			ans += processMul(match);
		}
	}
	std::cout << "res: " << ans;
}

void sol2() {
	std::vector<std::string> lines;
	parseFile("3/input.txt", lines);
	std::regex mulPattern("mul\\(\\d{1,3},\\d{1,3}\\)|do\\(\\)|don't\\(\\)");
	int ans{0};
	bool enabled{true};
	for(std::string line : lines){
		auto words_begin = std::sregex_iterator(line.begin(), line.end(), mulPattern);
		auto words_end = std::sregex_iterator();
		for(std::sregex_iterator i = words_begin; i != words_end; ++i){
			std::string match = (*i).str();
			if (match == "do()"){
				enabled = true;
			} else if (match == "don't()") {
				enabled = false;
			} else {
				if (enabled) {
					ans += processMul(match);
				}
			}
		}
	}
	std::cout << "res: " << ans;
}

int main(){
	sol2();
}
