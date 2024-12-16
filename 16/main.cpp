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

enum class Direction {UP, DOWN, RIGHT, LEFT};

std::pair<int, int> getDirectionDiff(Direction dir) {
	if (dir == Direction::UP) {
		return std::make_pair(-1,0);
	} else if (dir == Direction::DOWN) {
		return std::make_pair(1,0);
	} else if (dir == Direction::LEFT) {
		return std::make_pair(0,-1);
	} else {
		return std::make_pair(0,1);
	}
}

struct State {
	int x{};
	int y{};
	Direction dir{};
	long long score{};
};

struct DefaultState {
	int startX{};
	int startY{};
	int endX{};
	int endY{};
	std::vector<std::string> board{};
};

std::string getDirStr(const Direction dir) {
	if (dir == Direction::UP) {
		return "up";
	} else if (dir == Direction::DOWN) {
		return "down";
	} else if (dir == Direction::LEFT) {
		return "left";
	} else {
		return "right";
	}	
}

Direction rev(const Direction dir) {
	if (dir == Direction::UP) {
		return Direction::DOWN;
	} else if (dir == Direction::DOWN) {
		return Direction::UP;
	} else if (dir == Direction::LEFT) {
		return Direction::RIGHT;
	} else {
		return Direction::LEFT;
	}		
}

std::ostream & operator << (std::ostream & outs, const Direction & dir) {
	return outs << getDirStr(dir);
}

std::ostream & operator << (std::ostream & outs, const State & state) {
	outs << "State(" << state.x << "," << state.y << "," << getDirStr(state.dir) <<") = " << state.score << "\n";
	return outs;
}

std::ostream & operator << (std::ostream & outs, const DefaultState & state) {
	outs << "start(" << state.startX << "," << state.startY << ")" << " ";
	outs << "end(" << state.endX << "," << state.endY << ")" << "\n";
	outs << "==board==\n";
	for(int i{0}; i < static_cast<int>(state.board.size()) ; ++i){
		std::string line{state.board[static_cast<size_t>(i)]};
		for(int j{0} ; j < static_cast<int>(line.size());++j){
			if(i == state.startX && j == state.startY){
				std::cout << "@";
			} else if (i == state.endX && j == state.endY){
				std::cout << "E";			
			} else {
				std::cout << line[static_cast<size_t>(j)];
			}
		}
		std::cout << "\n";
	}
    return outs;
}

DefaultState getInitialState(std::vector<std::string> board) {
	int startX{0}, startY{0}, endX{0}, endY{0};
	for(int x{0}; x < static_cast<int>(board.size()) ; ++x){
		for(int y{0}; y < static_cast<int>(board[0].size()); ++y){
			if(board[static_cast<size_t>(x)][static_cast<size_t>(y)] == 'S'){
				board[static_cast<size_t>(x)][static_cast<size_t>(y)] = '.';
				startX = x;
				startY = y;
			}
			if(board[static_cast<size_t>(x)][static_cast<size_t>(y)] == 'E'){
				board[static_cast<size_t>(x)][static_cast<size_t>(y)] = '.';
				endX = x;
				endY = y;
			}
		}
	}
	return {startX, startY, endX, endY, board}; 
}

void parseFile(std::string filePath, DefaultState& state){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string line{};
	std::vector<std::string> board;
	while (std::getline(inFile, line)) {
		board.push_back(line);	
	}
	inFile.close();
	state = getInitialState(board);
}

bool inBound(const int x, const int y, const std::vector<std::string>& board){
	if ( x < 0 || x >= static_cast<int>(board.size()) || y < 0 || y >= static_cast<int>(board[0].size())){
		return false;
	}
	return true;
}

std::string getMapStr(State state, bool reverse = false) {
	if (reverse) {
		return std::to_string(state.x) + "-" +std::to_string(state.y) + "-" + getDirStr(rev(state.dir));
	} else {
		return std::to_string(state.x) + "-" +std::to_string(state.y) + "-" + getDirStr(state.dir);
	}
}

std::vector<Direction> getAdjacents(Direction dir) {
	if (dir == Direction::UP || dir == Direction::DOWN) {
		return {Direction::LEFT,Direction::RIGHT};
	} else {
		return {Direction::UP, Direction::DOWN};
	}		
}

std::vector<State> getNexts(const State state, const std::vector<std::string>& board, bool reverse = false){
	std::vector<State> ans;
	long long scoreAddition = reverse ? -1000 : 1000;
	for(Direction d: getAdjacents(state.dir)){
		ans.emplace_back(state.x,state.y,d,state.score+scoreAddition);
	}
	auto [xDiff, yDiff] = getDirectionDiff(state.dir);
	int newX{state.x + xDiff};
	int newY{state.y + yDiff};
	if(inBound(newX,newY,board) && board[static_cast<size_t>(newX)][static_cast<size_t>(newY)] == '.'){
		scoreAddition = reverse ? -1 : 1;
		ans.emplace_back(newX,newY,state.dir,state.score+scoreAddition);
	}
	return ans;
}

long long minScoreSearch(DefaultState defaultState, std::unordered_map<std::string, long long>& stateToScoreMap){
	std::deque<State> dq{};
	dq.emplace_back(defaultState.startX, defaultState.startY, Direction::RIGHT, 0);
	long long minScore{__LONG_LONG_MAX__};
	while(dq.size() > 0){
		State curr{dq.back()};
		dq.pop_back();
		std::string mapStr{getMapStr(curr)};
		if(stateToScoreMap.count(mapStr) == 0 || curr.score < stateToScoreMap[mapStr]){
			stateToScoreMap[mapStr] = curr.score;
		}
		if(curr.x == defaultState.endX && curr.y == defaultState.endY){
			minScore = std::min(curr.score, minScore);
			continue;
		}
		std::vector<State> nexts{getNexts(curr, defaultState.board)};
		for(State s: nexts){
			const std::string currMapStr{getMapStr(s)};
			if(stateToScoreMap.count(currMapStr) > 0 && s.score < stateToScoreMap[currMapStr]){
				dq.push_back(s);
			} else if(stateToScoreMap.count(currMapStr) == 0){
				dq.push_back(s);
			}
		}
	}
	return minScore;
}

void walkbackSeatCount(
	const std::vector<std::string>& board, State state,
	std::unordered_map<std::string, long long>& stateToScoreMap, 
	std::unordered_set<std::string>& seats){
	if (!inBound(state.x,state.y,board)) return;
	if(board[static_cast<size_t>(state.x)][static_cast<size_t>(state.y)] == '#') return;
	State reversedState{state.x,state.y,rev(state.dir),state.score};
	std::string mapStr{getMapStr(reversedState)};
	if (stateToScoreMap[mapStr] != state.score) return; // non best path
	seats.insert(std::to_string(state.x) + "-" + std::to_string(state.y));
	std::vector<State> nexts{getNexts(state, board, true)};
	for(State s: nexts){
		walkbackSeatCount(board, s, stateToScoreMap, seats);
	}
}

void sol1(){
	DefaultState state{};
	parseFile("16/input.txt", state);
	std::cout << state << "\n";
	// ["x-y-dir"] = 5
	std::unordered_map<std::string, long long> stateToScoreMap{};
	long long minScore{minScoreSearch(state, stateToScoreMap)};
	std::cout << "min score: " << minScore;
}

void sol2(){
	DefaultState defState{};
	parseFile("16/input.txt", defState);
	std::cout << defState << "\n";
	std::unordered_map<std::string, long long> stateToScoreMap{};
	long long minScore{minScoreSearch(defState, stateToScoreMap)};
	std::cout << "min score: " << minScore << "\n";

	// ["x-y"]
	std::unordered_set<std::string> seats{};
	State state{defState.endX, defState.endY, Direction::UP, minScore};
	walkbackSeatCount(defState.board, state ,stateToScoreMap, seats);
	state = {defState.endX, defState.endY, Direction::DOWN, minScore};
	walkbackSeatCount(defState.board, state ,stateToScoreMap, seats);
	state = {defState.endX, defState.endY, Direction::LEFT, minScore};
	walkbackSeatCount(defState.board, state ,stateToScoreMap, seats);
	state = {defState.endX, defState.endY, Direction::RIGHT, minScore};
	walkbackSeatCount(defState.board, state ,stateToScoreMap, seats);

	std::cout << "best seats: " << seats.size();
}

int main() {
	sol2();
}