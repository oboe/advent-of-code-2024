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

struct ClawMachine {
	std::pair<long long, long long> buttonA{};
	std::pair<long long, long long> buttonB{};
	std::pair<long long, long long> prizeLocation{};
};

std::ostream & operator << (std::ostream & outs, const ClawMachine& clawMachine) {
	outs << "A(" << clawMachine.buttonA.first << "," << clawMachine.buttonA.second << ") ";
	outs << "B(" << clawMachine.buttonB.first << "," << clawMachine.buttonB.second << ") ";
	outs << "Prize(" << clawMachine.prizeLocation.first << "," << clawMachine.prizeLocation.second << ")";
	return outs;
}

void parseFile(std::string filePath, std::vector<ClawMachine>& clawMachines, bool addBigNum = false){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string x{};
	int lineNum{0};
	ClawMachine curr{};
	while (std::getline(inFile, x)) {
		if (x.empty()) continue;
		if (lineNum == 0){
			std::vector<std::string> a{split<std::string>(x)};
			curr.buttonA = std::make_pair(stoi(a[2].substr(2,a[2].size()-3)),stoi(a[3].substr(2,a[3].size()-2)));
			++lineNum;
		} else if (lineNum == 1) {
			std::vector<std::string> b{split<std::string>(x)};
			curr.buttonB = std::make_pair(stoi(b[2].substr(2,b[2].size()-3)),stoi(b[3].substr(2,b[3].size()-2)));
			++lineNum;
		} else {
			std::vector<std::string> prize{split<std::string>(x)};
			curr.prizeLocation = std::make_pair(stoi(prize[1].substr(2,prize[1].size()-3)),stoi(prize[2].substr(2,prize[2].size()-2)));
			if (addBigNum){
				curr.prizeLocation.first += static_cast<long long>(1e13);
				curr.prizeLocation.second += static_cast<long long>(1e13);
			}
			clawMachines.push_back(curr);
			curr = {};
			lineNum = 0;
		}
	}
	inFile.close();
}

long long getPossibleCost(const ClawMachine machine){
	auto [a, b, prize] = machine;
    long long i = b.second * prize.first - b.first * prize.second;
    long long j = -a.second * prize.first + a.first * prize.second;
    long long det = (a.first * b.second) - (a.second * b.first);

    if (det == 0 || i % det != 0 || j % det != 0) {
        return 0ll;
    } else {
        return 3 * i / det + j / det;
    }
}

long long getCost(const ClawMachine machine){
	long long minCost{__LONG_LONG_MAX__};
	long long i{0},ix{0}, iy{0};
	long long j{machine.prizeLocation.first/machine.buttonB.first};
	j = std::min(j, machine.prizeLocation.second/machine.buttonB.second);
	std::unordered_map<std::string, long long> bCache;
	while(ix <= machine.prizeLocation.first && iy <= machine.prizeLocation.second){
		const long long xdiff{machine.prizeLocation.first - ix};
		const long long ydiff{machine.prizeLocation.second - iy};
		if(bCache.count(std::to_string(xdiff) + "-" +std::to_string(ydiff)) > 0){
			minCost = std::min(minCost, (i*3 + bCache[std::to_string(xdiff) + "-" +std::to_string(ydiff)]));
		} else {
			long long l{0}, r{j};
			while(l<r){
				const long long mid{(l+r)/2};
				const long long x{machine.buttonA.first * i + machine.buttonB.first * mid};
				const long long y{machine.buttonA.second * i + machine.buttonB.second * mid};
				bCache[std::to_string(machine.buttonB.first * mid) + "-" +std::to_string(machine.buttonB.second * mid)] = mid;
				if (machine.prizeLocation.first == x && machine.prizeLocation.second == y) {
					minCost = std::min(minCost, (i*3 + mid));
				} 
				if (x >= machine.prizeLocation.first || y >= machine.prizeLocation.second){
					r = mid;
				} else {
					l = mid + 1;
				}
			}
		}
		++i;
		ix += machine.buttonA.first;
		iy += machine.buttonA.second;
	}
	if (minCost == __LONG_LONG_MAX__) minCost = 0;
	return minCost;
}

void sol1(){
	std::vector<ClawMachine> clawMachines;
	parseFile("13/input.txt", clawMachines);
	long long totalCost{0};
	for (ClawMachine machine: clawMachines){
		totalCost += getCost(machine);
		std::cout << machine << " cost: " << totalCost << "\n";
	}
	std::cout << "total cost: " << totalCost;
}

void sol2(){
	std::vector<ClawMachine> clawMachines;
	parseFile("13/input.txt", clawMachines, true);
	long long totalCost{0};
	for (ClawMachine machine: clawMachines){
		totalCost += getPossibleCost(machine);
		std::cout << machine << " cost: " << totalCost << "\n";
	}
	std::cout << "total cost: " << totalCost;
}

int main() {
	sol2();
}