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
#include <deque>
#include <sstream>
#include <cassert>

void parseFile(std::string filePath, std::vector<std::string>& pattern) {
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string line{};
	while (std::getline(inFile, line)) {
		pattern.push_back(line);	
	}
	inFile.close();
}

int64_t getShortest(const std::string& presses, std::vector<std::string> pad, const int level);

struct State{
	int x, y;
	std::string presses;
};

const std::vector<std::string> keypad{
		{"X^A"},
		{"<v>"}
	};

const std::vector<std::string> numpad{
		{"789"},
		{"456"},
		{"123"},
		{"X0A"}
	}; 

std::pair<int,int> getPos(const std::vector<std::string>& pad, const char c){
	for (int xx{0}; xx < static_cast<int>(pad.size()); ++xx) {
		for (int yy{0}; yy < static_cast<int>(pad[0].size()); ++yy) {
			if (pad[static_cast<size_t>(xx)][static_cast<size_t>(yy)] == c) {
				return {xx,yy};
			}
		}
	}
	throw std::invalid_argument("can't find char " + std::string{c});
}

std::unordered_map<std::string, int64_t> mp;

std::string getMpStr(const int x, const int y, const int xx, const int yy, const int level, const bool isKeypad){
	std::string ans{(isKeypad) ? "keypad-" : "numpad-"};
	ans += std::to_string(x) + "-" + std::to_string(y) + "-";
	ans += std::to_string(xx) + "-" + std::to_string(yy) + "-" +std::to_string(level);
	return ans;
}

int64_t getShortestPath(const int x, const int y, const int xx, const int yy, const int level, const bool isKeypad = true) {
    const std::string mpStr = getMpStr(x, y, xx, yy, level, isKeypad);
    auto it = mp.find(mpStr);
    if (it != mp.end()) return it->second;
    int64_t answer = std::numeric_limits<int64_t>::max();
    std::deque<State> dq;
    dq.emplace_back( x, y, "" );
    while (!dq.empty()) {
        auto v = dq.front();
        dq.pop_front();
        if (v.x == xx && v.y == yy) {
            int64_t rec = getShortest(v.presses + "A", keypad, (isKeypad) ? level-1 : level);
            answer = std::min(answer, rec);
            continue;
        }
        if(isKeypad && (v.x == 0 && v.y == 0)) continue;
		if(!isKeypad && (v.x == 3 && v.y == 0)) continue;
		if (v.x < xx) dq.emplace_back( v.x + 1,v.y,v.presses + "v" );
		if (v.x > xx) dq.emplace_back( v.x - 1, v.y, v.presses + "^" );
		if (v.y < yy) dq.emplace_back( v.x, v.y + 1, v.presses + ">" );
		if (v.y > yy) dq.emplace_back( v.x, v.y - 1, v.presses + "<" );
    }
    mp[mpStr] = answer;
    return answer;
}

int64_t getShortest(const std::string& presses, const std::vector<std::string> pad, const int level) {
    if (level == 1) return static_cast<int64_t>(presses.length());
	const bool isKeypad{(pad.size() != 4)};
    int64_t result = 0;
	auto [x,y] = getPos(pad, 'A');
	for(char c: presses){
		auto [xx,yy] = getPos(pad, c);
		result += getShortestPath(x, y, xx, yy, level, (isKeypad) ? true : false);
        x = xx;
        y = yy;
	}
    return result;
}

int getDigits(const std::string s) {
	std::stringstream ss(s);
    int code;
    ss >> code;
    return code;	
}

void sol1(){
	std::vector<std::string> pattern;
	parseFile("21/input.txt", pattern);
    int64_t sum = 0;
	for(std::string s : pattern){
		sum += getShortest(s, numpad, 3) * getDigits(s);
	}
	assert(sum == 217662);
    std::cout << "sol1 ans: "<< sum << std::endl;	
}

void sol2(){
	std::vector<std::string> pattern;
	parseFile("21/input.txt", pattern);
    int64_t sum = 0;
	for(std::string s : pattern){
        sum += getShortest(s, numpad,26) * getDigits(s);
	}
	assert(sum == 263617786809000);
    std::cout << "sol2 ans: "<< sum << std::endl;	
}

int main() {
	sol1();
	sol2();
}