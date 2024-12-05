#include <iostream>
#include <fstream>

std::vector<int> splitIntoInts(const std::string& str) {
	std::vector<int> ans;
	unsigned long start{};
	unsigned long end{};
	while ( (start = str.find_first_not_of(" ", end)) != std::string::npos ) {
		end = str.find(" ", start);
		ans.push_back(stoi(str.substr(start, end-start)));
	}
	return ans;
}

void parseFile(std::string filePath, std::vector<std::vector<int>>& reports){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string x{};
	while (std::getline(inFile, x)) {
		std::vector<int> lineVals = splitIntoInts(x);
		reports.push_back(lineVals);
	}
	inFile.close();
}

bool isSafe(std::vector<int> report) {
	int last{};
	bool isDecreasing{true};
	for(unsigned long i{0}; i < report.size(); ++i){
		if(i == 0) {
			last = report[i];
			continue;
		}
		int diff = abs(last-report[i]);
		if (i == 1) {
			if ((diff > 3) || (diff == 0)){
				return false;
			}
			isDecreasing = (last > report[i]);
			last = report[i];
			continue;
		}
		if ((isDecreasing != (last > report[i])) || (diff > 3) || (diff == 0)){
			return false;
		}
		last = report[i];
	}
	return true;
}

void sol1() {
	std::vector<std::vector<int>> reports;
	parseFile("2/input.txt", reports);
	int ans{0};
	for(std::vector<int> report: reports){
		ans += isSafe(report);
	}
	std::cout << "num safe: " << ans << "\n";
}

void sol2() {
	std::vector<std::vector<int>> reports;
	parseFile("2/input.txt", reports);
	int ans{0};
	for(std::vector<int> report: reports){
		for(unsigned long i = 0 ; i < report.size() ; ++i){
			std::vector<int> clone {report};
			clone.erase(clone.begin() + static_cast<long>(i));
			if (isSafe(clone)) {
				++ans;
				break;
			}
		}
	}
	std::cout << "num safe: " << ans << "\n";
}

int main(){
	sol2();
}
