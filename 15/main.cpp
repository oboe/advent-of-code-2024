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
	int botX{};
	int botY{};
	std::vector<std::string> board{};
};

std::ostream & operator << (std::ostream & outs, const Direction & dir) {
	if (dir == Direction::UP) {
		return outs << "up";
	} else if (dir == Direction::DOWN) {
		return outs << "down";
	} else if (dir == Direction::LEFT) {
		return outs << "left";
	} else {
		return outs << "right";
	}
}

std::ostream & operator << (std::ostream & outs, const State & state) {
	outs << "bot(" << state.botX << "," << state.botY << ")" << "\n";
	outs << "==board==\n";
	for(int i{0}; i < static_cast<int>(state.board.size()) ; ++i){
		std::string line{state.board[static_cast<size_t>(i)]};
		for(int j{0} ; j < static_cast<int>(line.size());++j){
			if(i == state.botX && j == state.botY){
				std::cout << "@";
			} else {
				std::cout << line[static_cast<size_t>(j)];
			}
		}
		std::cout << "\n";
	}
    return outs;
}

State getInitialState(std::vector<std::string> board) {
	for(int x{0}; x < static_cast<int>(board.size()) ; ++x){
		for(int y{0}; y < static_cast<int>(board[0].size()); ++y){
			if(board[static_cast<size_t>(x)][static_cast<size_t>(y)] == '@'){
				board[static_cast<size_t>(x)][static_cast<size_t>(y)] = '.';
				return {x,y,board};
			}
		}
	}
	return {1000000,1000000,board}; 
}

void parseFile(std::string filePath, State& state, std::string& actions){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string line{};
	bool isBoardLines{true};
	std::vector<std::string> board;
	while (std::getline(inFile, line)) {
		if (line.empty()) isBoardLines = false;
		if (isBoardLines) {
			board.push_back(line);
		} else {
			actions += line;
		}
	}
	inFile.close();
	state = getInitialState(board);
}

std::vector<Direction> parseActions(std::string_view actionString){
	std::vector<Direction> directions{};
	auto parse = [&directions](const char c) {
		if(c=='<'){
			directions.push_back(Direction::LEFT);
		} else if (c == '>') {
			directions.push_back(Direction::RIGHT);
		}else if (c == '^') {
			directions.push_back(Direction::UP);
		}else { //down
			directions.push_back(Direction::DOWN);
		}
	};
	std::for_each(actionString.begin(), actionString.end(), parse);
	return directions;
}

bool inBound(const int x, const int y, const std::vector<std::string>& board){
	if ( x < 0 || x >= static_cast<int>(board.size()) || y < 0 || y >= static_cast<int>(board[0].size())){
		return false;
	}
	return true;
}

void moveBlock(State& state, const int x, const int y,const Direction direction, const char block = '.'){
	if(!inBound(x,y,state.board)) return;
	const char currPos{state.board[static_cast<size_t>(x)][static_cast<size_t>(y)]};
	if(currPos == '#') return;
	state.board[static_cast<size_t>(x)][static_cast<size_t>(y)] = block;
	if(currPos == '.') return;
	if (direction == Direction::LEFT || direction == Direction::RIGHT || currPos == 'O') {
		auto [xDiff, yDiff] = getDirectionDiff(direction);
		moveBlock(state, x + xDiff, y + yDiff, direction, currPos);
	} else {
		auto [xDiff, yDiff] = getDirectionDiff(direction);
		char blockOtherside = (currPos == '[') ? ']' : '[';
		int blockOthersideDiff = (currPos == '[') ? 1 : -1;
		state.board[static_cast<size_t>(x)][static_cast<size_t>(y+blockOthersideDiff)] = '.';
		moveBlock(state, x + xDiff, y + yDiff, direction, currPos);
		moveBlock(state, x + xDiff, y + yDiff + blockOthersideDiff, direction, blockOtherside);
	}
}

bool isPushable(const State& state, const int x, const int y,const Direction direction){
	if(!inBound(x,y,state.board)) return false;
	const char currPos{state.board[static_cast<size_t>(x)][static_cast<size_t>(y)]};
	if(currPos == '#') return false;
	if(currPos == '.') return true;
	if (direction == Direction::LEFT || direction == Direction::RIGHT || currPos == 'O') {
		auto [xDiff, yDiff] = getDirectionDiff(direction);
		return isPushable(state, x + xDiff, y + yDiff, direction);	
	} else {
		auto [xDiff, yDiff] = getDirectionDiff(direction);
		int blockOthersideDiff = (currPos == '[') ? 1 : -1;
		return isPushable(state, x + xDiff, y + yDiff, direction) && isPushable(state, x + xDiff, y + yDiff + blockOthersideDiff, direction);	
	}
}

void moveRobot(State& state, const Direction direction) {
	auto [xDiff, yDiff] = getDirectionDiff(direction);
	if(!isPushable(state, state.botX+xDiff, state.botY+yDiff, direction)) return;
	state.botX += xDiff;
	state.botY += yDiff;
	moveBlock(state, state.botX, state.botY, direction);
}

long long getTotalGPSScore(const State state){
	long long ans{0};
	for(int x{0}; x < static_cast<int>(state.board.size()) ; ++x){
		for(int y{0}; y < static_cast<int>(state.board[0].size()); ++y){
			const char c{state.board[static_cast<size_t>(x)][static_cast<size_t>(y)]};
			if(c == 'O' || c == '['){
				ans += (100 * x) + y;
			}
		}
	}
	return ans;
}

void makeWarehouseFat(State& state){
	state.botY *= 2;
	std::vector<std::string> newBoard{};
	for(int x{0}; x < static_cast<int>(state.board.size()) ; ++x){
		std::string newLine{};
		for(int y{0}; y < static_cast<int>(state.board[0].size()); ++y){
			const char c{state.board[static_cast<size_t>(x)][static_cast<size_t>(y)]};
			if (c == 'O') newLine += "[]";
			if (c == '#') newLine += "##";
			if (c == '.') newLine += "..";
		}
		newBoard.push_back(newLine);
	}
	state.board = newBoard;	
}


void sol1(){
	State state{};
	std::string actions{};
	parseFile("15/input.txt", state, actions);
	std::cout << state << "\n";
	std::cout << actions << "\n";
	std::vector<Direction> directions{parseActions(actions)};
	for(Direction d: directions){
		moveRobot(state, d);
	}
	long long total{getTotalGPSScore(state)};
	std::cout << "total: " << total;
}

void sol2(){
	State state{};
	std::string actions{};
	parseFile("15/input.txt", state, actions);
	makeWarehouseFat(state);
	std::cout << state << "\n";
	std::cout << actions << "\n";
	std::vector<Direction> directions{parseActions(actions)};
	for(Direction d: directions){
		moveRobot(state, d);
	}
	long long total{getTotalGPSScore(state)};
	std::cout << "total: " << total;
}

int main() {
	sol2();
}