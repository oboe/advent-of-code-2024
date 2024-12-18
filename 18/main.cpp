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
T fromString(const std::string& word) {
	if constexpr (std::is_same<T, int>::value) {
		return T(stoi(word));
	} else if constexpr (std::is_same<T, long long>::value) {
		return T(stoll(word));
	} else if constexpr (std::is_same<T, double>::value) {
		return T(stod(word));
	} else {
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

template <class T>
std::vector<T> parseNumbers(const std::string& line){
	std::vector<T> ans;
	std::regex pattern("-?\\d+");
	auto words_begin = std::sregex_iterator(line.begin(), line.end(), pattern);
	auto words_end = std::sregex_iterator();
	for(std::sregex_iterator i = words_begin; i != words_end; ++i){
		std::string match = (*i).str();
		ans.push_back(fromString<T>(match));
	}
	return ans;
}

struct State {
	int x{};
	int y{};
	long long score{};
	std::vector<std::string> board{};
};

std::ostream & operator << (std::ostream & outs, const State & state) {
	outs << "bot(" << state.x << "," << state.y << ")" << "\n";
	outs << "==board==\n";
	for(int i{0}; i < static_cast<int>(state.board.size()) ; ++i){
		std::string line{state.board[static_cast<size_t>(i)]};
		for(int j{0} ; j < static_cast<int>(line.size());++j){
			if(i == state.x && j == state.y){
				std::cout << "@";
			} else {
				std::cout << line[static_cast<size_t>(j)];
			}
		}
		std::cout << "\n";
	}
    return outs;
}

void parseFile(std::string filePath, std::vector<std::pair<int,int>>& positions){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string line{};
	while (std::getline(inFile, line)) {
		std::vector<int> nums{parseNumbers<int>(line)};
		positions.emplace_back(nums[1], nums[0]);
	}
	inFile.close();
}

bool inBound(const int x, const int y, const std::vector<std::string>& board){
	if ( x < 0 || x >= static_cast<int>(board.size()) || y < 0 || y >= static_cast<int>(board[0].size())){
		return false;
	}
	return true;
}

bool canMove(const int x, const int y, const std::vector<std::string>& board){
	if ( !inBound(x,y,board) || (inBound(x,y,board) && (board[static_cast<size_t>(x)][static_cast<size_t>(y)] != '.')) ){
		return false;
	}
	return true;
}

std::vector<std::string> buildBoard(const std::vector<std::pair<int,int>>& positions, const size_t xSize, const size_t ySize, const int time=1024){
	std::vector<std::string>board = std::vector<std::string>(xSize, std::string(ySize,'.'));
	int count{0};
	int x{},y{};
	for(std::pair<int,int> p : positions){
		if (count >= time) break;
		x = p.first;
		y = p.second;
		board[static_cast<size_t>(p.first)][static_cast<size_t>(p.second)] = '#';
		++count;
	}
	std::cout << "trying last: "<< y << "," << x << "\n";
	return board;
}

std::vector<State> getNexts(const State curr){
	std::vector<State> ans;
	if(canMove(curr.x+1,curr.y,curr.board)) ans.emplace_back(curr.x+1,curr.y,curr.score+1,curr.board);
	if(canMove(curr.x-1,curr.y,curr.board)) ans.emplace_back(curr.x-1,curr.y,curr.score+1,curr.board);
	if(canMove(curr.x,curr.y+1,curr.board)) ans.emplace_back(curr.x,curr.y+1,curr.score+1,curr.board);
	if(canMove(curr.x,curr.y-1,curr.board)) ans.emplace_back(curr.x,curr.y-1,curr.score+1,curr.board);
	return ans;
}

long long minPathSearch(const State defaultState, std::unordered_map<std::string, long long>& stateToScoreMap, const size_t xSize, const size_t ySize){
	std::deque<State> dq{};
	dq.emplace_back(0, 0, 0,defaultState.board);
	long long minScore{__LONG_LONG_MAX__};
	while(dq.size() > 0){
		State curr{dq.back()};
		dq.pop_back();
		std::string mapStr{std::to_string(curr.x) + "-"+ std::to_string(curr.y)};
		if(stateToScoreMap.count(mapStr) == 0 || curr.score < stateToScoreMap[mapStr]){
			stateToScoreMap[mapStr] = curr.score;
		}
		if(curr.x == (static_cast<int>(xSize)-1) && curr.y == (static_cast<int>(ySize)-1)){
			minScore = std::min(curr.score, minScore);
			continue;
		}
		std::vector<State> nexts{getNexts(curr)};
		for(State s: nexts){
			const std::string currMapStr{std::to_string(s.x) + "-"+ std::to_string(s.y)};
			if(stateToScoreMap.count(currMapStr) > 0 && s.score < stateToScoreMap[currMapStr]){
				dq.push_back(s);
			} else if(stateToScoreMap.count(currMapStr) == 0){
				dq.push_back(s);
			}
		}
	}
	return minScore;
}


void sol1(){
	size_t xSize{71}, ySize{71};
	std::vector<std::pair<int,int>> positions{};
	parseFile("18/input.txt", positions);
	std::vector<std::string> board{buildBoard(positions,xSize,ySize)};
	for(auto s: board){
		std::cout << s << "\n";
	}
	State defaultState{0,0,0,board};
	std::unordered_map<std::string, long long> stateToScoreMap;
	long long ans{minPathSearch(defaultState, stateToScoreMap, xSize, ySize)};
	std::cout << "min path: " << ans;
}

void sol2(){
	size_t xSize{71}, ySize{71};
	std::vector<std::pair<int,int>> positions{};
	parseFile("18/input.txt", positions);
	int l{0}, r{static_cast<int>(positions.size())};
	while(l < r){
		int mid{l + (r-l)/2};
		std::vector<std::string> board{buildBoard(positions,xSize,ySize,mid)};
		State defaultState{0,0,0,board};
		std::unordered_map<std::string, long long> stateToScoreMap;
		long long ans{minPathSearch(defaultState, stateToScoreMap, xSize, ySize)};
		if (ans == __LONG_LONG_MAX__) std::cout << "time "<< mid <<" was impossible!\n";
		if (ans != __LONG_LONG_MAX__){
			l = mid + 1;
		} else{
			r = mid;
		}
	}
}

int main() {
	sol2();
}