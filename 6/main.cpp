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

enum class Direction {UP, DOWN, RIGHT, LEFT};

std::pair<unsigned long, unsigned long> getDirectionDiff(Direction dir) {
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

Direction getDirectionNext(Direction dir) {
	if (dir == Direction::UP) {
		return Direction::RIGHT;
	} else if (dir == Direction::DOWN) {
		return Direction::LEFT;
	} else if (dir == Direction::LEFT) {
		return Direction::UP;
	} else {
		return Direction::DOWN;
	}
}

struct State {
	unsigned long guardX{};
	unsigned long guardY{};
	Direction guardDirection{};
	bool guardLeftBoard{};
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
	outs << "guard(" << state.guardX << "," << state.guardY << "," << state.guardDirection << ")" << "\n";
	outs << "==board==\n";
	for(std::string line : state.board){
		outs << line << "\n";
	}
	outs << "\n";
    return outs;
}

State getInitialState(std::vector<std::string> board) {
	for(unsigned long x{0}; x < board.size() ; ++x){
		for(unsigned long y{0}; y < board[0].size(); ++y){
			if(board[x][y] == '^'){
				return {x,y,Direction::UP,false, board};
			}
		}
	}
	return {1000000,1000000,Direction::UP,false, board}; 
}

State& computeNext(State& input){
	if (input.guardLeftBoard) return input;
	std::pair<unsigned long, unsigned long> diff{getDirectionDiff(input.guardDirection)};
	const unsigned long newX{input.guardX + diff.first};
	const unsigned long newY{input.guardY + diff.second};
	bool isBlocked{false};
	bool leftBoard{false};
	if (newX < 0 || newX >= input.board.size() || newY < 0 || newY >= input.board[0].size()){
		leftBoard = true;
	} else {
		if (input.board[newX][newY] == '#') {
			isBlocked = true;
		}	
	}
	input.board[input.guardX][input.guardY] = 'X';
	if (isBlocked) {
		input.guardDirection = getDirectionNext(input.guardDirection);
	} else {
		input.guardX = newX;
		input.guardY = newY;
	}
	input.guardLeftBoard = leftBoard;
	return input;
}

int countPositions(std::vector<std::string> board){
	int count{0};
	for(unsigned long x{0}; x < board.size() ; ++x){
		for(unsigned long y{0}; y < board[0].size(); ++y){
			if(board[x][y] == 'X'){
				++count;
			}
		}
	}
	return count;
}

bool computeIsLoopingBoard(std::vector<std::string> initialBoard){
	State state = getInitialState(initialBoard);
	std::vector<std::string> lastBoard{};
	int sameCount{0};
	while(true) {
		if(sameCount > 100){
			break;
		}
		computeNext(state);
		if (state.board == lastBoard) {
			++sameCount;
		} else {
			sameCount = 0;
		}
		lastBoard = state.board;
	}
	return !state.guardLeftBoard;
}

std::vector<std::pair<unsigned long,unsigned long>> getObstructionLocations(std::vector<std::string> initialBoard){
	State state = getInitialState(initialBoard);
	std::cout << state;
	std::vector<std::string> lastBoard{};
	int sameCount{0};
	while(true) {
		if(sameCount > 100){
			break;
		}
		state = computeNext(state);
		if (state.board == lastBoard) {
			++sameCount;
		} else {
			sameCount = 0;
		}
		lastBoard = state.board;
	}
	std::vector<std::pair<unsigned long,unsigned long>> ans{};
	for(unsigned long x{0}; x < initialBoard.size() ; ++x){
		for(unsigned long y{0}; y < initialBoard[0].size(); ++y){
			if(initialBoard[x][y] != '^' && state.board[x][y] == 'X'){
				ans.push_back(std::make_pair(x,y));
			}
		}
	}
	return ans;	
}

void sol1() {
	std::vector<std::string> initialBoard;
	parseFile("6/input.txt", initialBoard);
	State state = getInitialState(initialBoard);
	std::cout << state;
	std::vector<std::string> lastBoard{};
	int sameCount{0};
	while(true) {
		if(sameCount > 100){
			break;
		}
		state = computeNext(state);
		if (state.board == lastBoard) {
			++sameCount;
		} else {
			sameCount = 0;
		}
		lastBoard = state.board;
	}
	std::cout << state;
	std::cout << "count: " << countPositions(state.board);
}

void sol2() {
	std::vector<std::string> initialBoard;
	parseFile("6/input.txt", initialBoard);
	State state = getInitialState(initialBoard);
	const std::vector<std::pair<unsigned long,unsigned long>> obstructionLocations{getObstructionLocations(initialBoard)};
	std::cout << "obstruction locations: " << obstructionLocations.size() << "\n";
	int count{0};
	int loopCount{0};
	for(std::pair<unsigned long,unsigned long> obstructionLocation: obstructionLocations){
		++loopCount;
		if (loopCount%100 == 0){
			std::cout << "loopcount: " << loopCount << ",curr: " << count <<"\n";
		}
		std::vector<std::string> newBoard = initialBoard;
		newBoard[obstructionLocation.first][obstructionLocation.second] = '#';
		if (computeIsLoopingBoard(newBoard)){
			++count;
		}
	}
	std::cout << "count: " << count;
}

int main(){
	sol2();
}
