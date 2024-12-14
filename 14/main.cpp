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

struct Robot {
	std::pair<long long, long long> start{};
	std::pair<long long, long long> velocity{};
};

std::ostream & operator << (std::ostream & outs, const Robot& robot) {
	outs << "Start(" << robot.start.first << "," << robot.start.second << ")";
	outs << " Vel(" << robot.velocity.first << "," << robot.velocity.second << ")";
	return outs;
}

void parseFile(std::string filePath, std::vector<Robot>& robots){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string line{};
	while (std::getline(inFile, line)) {
		if (line.empty()) continue;
		std::vector<int> numbers{parseNumbers<int>(line)};
		robots.emplace_back(std::make_pair(numbers[0],numbers[1]),std::make_pair(numbers[2],numbers[3]));
	}
	inFile.close();
}

int count(std::vector<std::vector<int>> board, int xStart, int yStart, int xEnd, int yEnd) {
	int ans{0};
	for(int i{xStart} ; i < xEnd ; ++i){
		for(int j{yStart} ; j < yEnd ; ++j){
			ans += board[static_cast<size_t>(i)][static_cast<size_t>(j)];
		}
	}
	return ans;
}

void updateBoard(std::vector<std::vector<int>>& board, std::vector<Robot> robots, long long xBounds, long long yBounds, long long seconds=100ll){
	for(Robot r: robots) {
		const long long xNew{(r.start.first + r.velocity.first * seconds + xBounds * 100000ll) % xBounds};
		const long long yNew{(r.start.second + r.velocity.second * seconds + yBounds * 1000000ll) % yBounds};
		++board[static_cast<size_t>(xNew)][static_cast<size_t>(yNew)];
	}	
}

void wipeBoard(std::vector<std::vector<int>>& board){
	for(int i{0} ; i < static_cast<int>(board.size()) ; ++i){
		for(int j{0} ; j < static_cast<int>(board[0].size()) ; ++j){
			board[static_cast<size_t>(i)][static_cast<size_t>(j)] = 0;
		}
	}
}

void printBoard(std::vector<std::vector<int>>& board, std::string metadata = "") {
	std::cout << metadata << "\n";
	for(std::vector<int> vv : board){
		for(int ii : vv){
			if (ii == 0) {
				std::cout << ".";
			} else {
				std::cout << ii;
			}
		}
		std::cout << "\n";
	}
}

bool inBound(long long x, long long y, std::vector<std::vector<int>>& board){
	if ( x < 0 || x >= static_cast<long long>(board.size())){
		return false;
	}
	if ( y < 0 || y >= static_cast<long long>(board[0].size())){
		return false;
	}
	return true;
}

long long countConnected(std::vector<std::vector<int>>& board){
	long long ans{0};
	for(int i{0} ; i < static_cast<int>(board.size()) ; ++i){
		for(int j{0} ; j < static_cast<int>(board[0].size()) ; ++j){
			bool isolated{true};
			if (inBound(i+1,j,board) && board[static_cast<size_t>(i+1)][static_cast<size_t>(j)] != 0) isolated = false;
			if (inBound(i-1,j,board) && board[static_cast<size_t>(i-1)][static_cast<size_t>(j)] != 0) isolated = false;
			if (inBound(i,j+1,board) && board[static_cast<size_t>(i)][static_cast<size_t>(j+1)] != 0) isolated = false;
			if (inBound(i,j-1,board) && board[static_cast<size_t>(i)][static_cast<size_t>(j-1)] != 0) isolated = false;
			if (inBound(i+1,j+1,board) && board[static_cast<size_t>(i+1)][static_cast<size_t>(j+1)] != 0) isolated = false;
			if (inBound(i-1,j-1,board) && board[static_cast<size_t>(i-1)][static_cast<size_t>(j-1)] != 0) isolated = false;
			if (!isolated) ans += board[static_cast<size_t>(i)][static_cast<size_t>(j)];
		}
	}
	return ans;
}

bool isNoisy(std::vector<std::vector<int>>& board, long long xBounds, long long yBounds){
	const long long connected{countConnected(board)};
	const long long total{xBounds*yBounds};
	return ((total/connected) > 30ll);
}

void sol1(){
	std::vector<Robot> robots;
	parseFile("14/input.txt", robots);
	constexpr long long xBounds{101}; //wide
	constexpr long long yBounds{103}; //tall
	std::vector<std::vector<int>> board(xBounds, std::vector<int>(yBounds, 0));
	updateBoard(board, robots, xBounds, yBounds);
	printBoard(board);
	int ans{1};
	const int xMidEnd{xBounds/2};
	const int xMidStart{(xBounds%2==0) ? xMidEnd : xMidEnd + 1};
	const int yMidEnd{yBounds/2};
	const int yMidStart{(yBounds%2==0) ? yMidEnd : yMidEnd + 1};
	ans *= count(board,0,0,xMidEnd, yMidEnd);
	ans *= count(board,0,yMidStart,xMidEnd,yBounds);
	ans *= count(board,xMidStart,0,xBounds,yMidEnd);
	ans *= count(board,xMidStart,yMidStart,xBounds,yBounds);
	std::cout << "safety factor: " << ans;
}

void sol2(){
	std::vector<Robot> robots;
	parseFile("14/input.txt", robots);
	constexpr long long xBounds{101}; //wide
	constexpr long long yBounds{103}; //tall
	std::vector<std::vector<int>> board(xBounds, std::vector<int>(yBounds, 0));
	long long seconds{0};
	while(true) {
		updateBoard(board, robots, xBounds, yBounds, seconds);
		if (!isNoisy(board, xBounds, yBounds)){
			printBoard(board, std::to_string(seconds) + " seconds");
		}
		wipeBoard(board);
		seconds += 1;
	}
}

int main() {
	sol2();
}