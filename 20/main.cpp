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
	int cheatingPicosecondsLeft{};
	long long score{};
	int cheatx{0};
	int cheaty{0};
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

std::string getMapStr(State state) {
	return "(" + std::to_string(state.x) + "," +std::to_string(state.y) + ")" + std::to_string(state.cheatingPicosecondsLeft) + "(" + std::to_string(state.cheatx) + "," +std::to_string(state.cheaty) + ")";	
}

std::ostream & operator << (std::ostream & outs, const Direction & dir) {
	return outs << getDirStr(dir);
}

std::ostream & operator << (std::ostream & outs, const State & state) {
	outs << "State"<<getMapStr(state) << " = " << state.score << "\n";
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

std::vector<State> getNexts(const State state, const std::vector<std::string>& board, int defaultCheatTime){
	std::vector<State> ans;
	if(state.cheatingPicosecondsLeft == 0 || state.cheatingPicosecondsLeft == defaultCheatTime){
		if(inBound(state.x+1,state.y,board) && (board[static_cast<size_t>(state.x+1)][static_cast<size_t>(state.y)] == '.')) ans.emplace_back(state.x+1,state.y,state.cheatingPicosecondsLeft,state.score+1,-1,-1);
		if(inBound(state.x-1,state.y,board) && (board[static_cast<size_t>(state.x-1)][static_cast<size_t>(state.y)] == '.')) ans.emplace_back(state.x-1,state.y,state.cheatingPicosecondsLeft,state.score+1,-1,-1);	
		if(inBound(state.x,state.y+1,board) && (board[static_cast<size_t>(state.x)][static_cast<size_t>(state.y+1)] == '.')) ans.emplace_back(state.x,state.y+1,state.cheatingPicosecondsLeft,state.score+1,-1,-1);
		if(inBound(state.x,state.y-1,board) && (board[static_cast<size_t>(state.x)][static_cast<size_t>(state.y-1)] == '.')) ans.emplace_back(state.x,state.y-1,state.cheatingPicosecondsLeft,state.score+1,-1,-1);
	}
	if (state.cheatingPicosecondsLeft >= 1 || state.cheatingPicosecondsLeft == defaultCheatTime) {
		const int cheatx{(state.cheatingPicosecondsLeft != defaultCheatTime) ? state.cheatx : state.x};
		const int cheaty{(state.cheatingPicosecondsLeft != defaultCheatTime) ? state.cheaty : state.y};
		if(inBound(state.x+1,state.y,board)) ans.emplace_back(state.x+1,state.y,state.cheatingPicosecondsLeft-1,state.score+1,cheatx,cheaty);
		if(inBound(state.x-1,state.y,board)) ans.emplace_back(state.x-1,state.y,state.cheatingPicosecondsLeft-1,state.score+1,cheatx,cheaty);	
		if(inBound(state.x,state.y+1,board)) ans.emplace_back(state.x,state.y+1,state.cheatingPicosecondsLeft-1,state.score+1,cheatx,cheaty);
		if(inBound(state.x,state.y-1,board)) ans.emplace_back(state.x,state.y-1,state.cheatingPicosecondsLeft-1,state.score+1,cheatx,cheaty);	
	}
	return ans;
}

long long search(const DefaultState defaultState, std::unordered_map<std::string, long long>& stateToScoreMap, const int defaultCheatTime = 2){
	std::deque<State> dq{};
	dq.emplace_back(defaultState.startX, defaultState.startY, defaultCheatTime, 0, -1, -1);
	dq.emplace_back(defaultState.startX, defaultState.startY, 0, 0, -1, -1);
	long long minScore{__LONG_LONG_MAX__};
	while(dq.size() > 0){
		const State curr{dq.back()};
		dq.pop_back();
		const std::string mapStr{getMapStr(curr)};
		if(stateToScoreMap.count(mapStr) == 0 || curr.score < stateToScoreMap[mapStr]){
			stateToScoreMap[mapStr] = curr.score;
		}
		const State currEndHere{curr.x,curr.y,0,curr.score,curr.cheatx,curr.cheaty};
		const std::string endStr{getMapStr(currEndHere)};
		if(curr.cheatingPicosecondsLeft < defaultCheatTime && (stateToScoreMap.count(endStr) == 0 || curr.score < stateToScoreMap[endStr])) {
			stateToScoreMap[endStr] = curr.score;
		}
		std::vector<State> nexts{getNexts(curr, defaultState.board, defaultCheatTime)};
		if(curr.cheatingPicosecondsLeft < defaultCheatTime) {
			std::vector<State> nextsEndHere{getNexts(currEndHere, defaultState.board, defaultCheatTime)};
			nexts.insert(nexts.end(), nextsEndHere.begin(), nextsEndHere.end());	
		}
		for(State s: nexts){
			const std::string currMapStr{getMapStr(s)};
			if((stateToScoreMap.count(currMapStr) == 0) || (stateToScoreMap.count(currMapStr) > 0 && s.score < stateToScoreMap[currMapStr])){
				dq.push_back(s);
			}
		}
	}
	return minScore;
}

void pp(const std::unordered_map<long long, long long>& cheatMap){
	std::vector<std::pair<long long, long long>> ss;
	for(auto [k,v]: cheatMap){
		ss.emplace_back(k,v);
	}
	sort(ss.begin(),ss.end());
	std::reverse(ss.begin(), ss.end());
	for(auto [k,v] : ss){
		if(k >= 50) std::cout << "There are " << v << " cheats that save " << k << " picoseconds.\n";
	}
}

void countCheats(std::unordered_map<long long, long long>& cheatMap, const DefaultState state, std::unordered_map<std::string, long long>& stateToScoreMap ,const int cheatTime = 2){
	for(int i{0}; i < static_cast<int>(state.board.size()) ; ++i){
		for(int j{0}; j < static_cast<int>(state.board[0].size()) ; ++j){
			if(state.board[static_cast<size_t>(i)][static_cast<size_t>(j)] == '#') continue;
			const std::string nonCheatStr{getMapStr(State{i,j,cheatTime,0,-1,-1})};
			for(int x{0}; x < static_cast<int>(state.board.size()) ; ++x){
				for(int y{0}; y < static_cast<int>(state.board[0].size()) ; ++y){
					const std::string cheatStr{getMapStr(State{i,j,0,0,x,y})};
					if(stateToScoreMap.count(cheatStr)>0 && stateToScoreMap.count(nonCheatStr)>0){
						const long long diff{stateToScoreMap[nonCheatStr] - stateToScoreMap[cheatStr]};
						if (diff > 0) ++cheatMap[diff];
					}
				}
			}
		}
	}
}

void sol1(){
	DefaultState state{};
	parseFile("20/input.txt", state);
	std::cout << state << "\n";
	std::unordered_map<std::string, long long> stateToScoreMap{};
	search(state, stateToScoreMap);
	std::unordered_map<long long, long long> cheatMap{};
	countCheats(cheatMap, state, stateToScoreMap, 2);
	pp(cheatMap);
	long long ans{0};
	for(auto [k,v]: cheatMap){
		if(k >= 100) ans += v;
	}
	std::cout << "cheats above 100: " << ans;
}

void sol2(){
	DefaultState state{};
	parseFile("20/input.txt", state);
	std::cout << state << "\n";
	std::unordered_map<std::string, long long> stateToScoreMap{};
	search(state, stateToScoreMap, 20);
	std::unordered_map<long long, long long> cheatMap{};
	countCheats(cheatMap, state, stateToScoreMap, 20);
	pp(cheatMap);
	long long ans{0};
	for(auto [k,v]: cheatMap){
		if(k >= 100) ans += v;
	}
	std::cout << "cheats above 100: " << ans;
}

int main() {
	sol2();
}