#include <iostream>
#include <fstream>

void parseFile(std::string filePath, std::vector<int>& l, std::vector<int>& r){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string x{};
	bool isL{true};
	while (inFile >> x) {
		if (isL){
			l.push_back(std::stoi(x));
			isL = false;
		} else {
			r.push_back(std::stoi(x));
			isL = true;
		}
	}
	inFile.close();
}

void sol1() {
	std::vector<int> l;
	std::vector<int> r;
	parseFile("1/input.txt", l, r);
	sort(l.begin(), l.end());
	sort(r.begin(), r.end());
	int totalDistance{0};
	for (unsigned long i{0}; i < l.size(); i++) {
		totalDistance += abs(l[i] - r[i]);
	}
	std::cout << "total distance: " << totalDistance;
}

void sol2() {
	std::vector<int> l;
	std::vector<int> r;
	parseFile("1/input.txt", l, r);
	std::unordered_map<int,int> rMap;
	for(int val: r) {
		rMap[val]++;
	}
	int similarityScore{0};
	for (int val: l) {
		similarityScore += val * rMap[val];
	}
	std::cout << "similarity: " << similarityScore;
}

int main(){
	sol2();
}
