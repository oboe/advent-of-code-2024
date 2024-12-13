#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>


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

bool inBound(long x, long y, std::vector<std::string>& board){
	if ( x < 0 || x >= static_cast<long>(board.size())){
		return false;
	}
	if ( y < 0 || y >= static_cast<long>(board[0].size())){
		return false;
	}
	return true;
}

int getArea(long x, long y, char last, std::vector<std::string>& board, std::vector<std::vector<bool>>& visited){
	if (!inBound(x,y,board)) return 0;
	if (board[static_cast<size_t>(x)][static_cast<size_t>(y)] != last) return 0;
	if (visited[static_cast<size_t>(x)][static_cast<size_t>(y)]) return 0;
	int ans{1};
	visited[static_cast<size_t>(x)][static_cast<size_t>(y)] = true;
	ans += getArea(x+1,y,last,board,visited);
	ans += getArea(x-1,y,last,board,visited);
	ans += getArea(x,y+1,last,board,visited);
	ans += getArea(x,y-1,last,board,visited);
	return ans;
}

int countSides(long x, long y, char last,std::vector<std::string>& board){
	int sides{0};
	if(!inBound(x+1,y,board) || ( inBound(x+1,y,board) && board[static_cast<size_t>(x+1)][static_cast<size_t>(y)] != last)) ++sides;
	if(!inBound(x-1,y,board) || ( inBound(x-1,y,board) && board[static_cast<size_t>(x-1)][static_cast<size_t>(y)] != last)) ++sides;
	if(!inBound(x,y+1,board) || ( inBound(x,y+1,board) && board[static_cast<size_t>(x)][static_cast<size_t>(y+1)] != last)) ++sides;
	if(!inBound(x,y-1,board) || ( inBound(x,y-1,board) && board[static_cast<size_t>(x)][static_cast<size_t>(y-1)] != last)) ++sides;
	return sides;
}

int countCorners(long x, long y, char last,std::vector<std::string>& board){
	int corners{0};
	bool up{false},down{false},left{false},right{false},uleft{false},uright{false},dleft{false},dright{false};
	if(!inBound(x+1,y,board) || ( inBound(x+1,y,board) && board[static_cast<size_t>(x+1)][static_cast<size_t>(y)] != last)) down = true;
	if(!inBound(x-1,y,board) || ( inBound(x-1,y,board) && board[static_cast<size_t>(x-1)][static_cast<size_t>(y)] != last)) up = true;
	if(!inBound(x,y+1,board) || ( inBound(x,y+1,board) && board[static_cast<size_t>(x)][static_cast<size_t>(y+1)] != last)) right = true;
	if(!inBound(x,y-1,board) || ( inBound(x,y-1,board) && board[static_cast<size_t>(x)][static_cast<size_t>(y-1)] != last)) left = true;

	if(!inBound(x+1,y+1,board) || ( inBound(x+1,y+1,board) && board[static_cast<size_t>(x+1)][static_cast<size_t>(y+1)] != last)) dright = true;
	if(!inBound(x-1,y+1,board) || ( inBound(x-1,y+1,board) && board[static_cast<size_t>(x-1)][static_cast<size_t>(y+1)] != last)) uright = true;
	if(!inBound(x+1,y-1,board) || ( inBound(x+1,y-1,board) && board[static_cast<size_t>(x+1)][static_cast<size_t>(y-1)] != last)) dleft = true;
	if(!inBound(x-1,y-1,board) || ( inBound(x-1,y-1,board) && board[static_cast<size_t>(x-1)][static_cast<size_t>(y-1)] != last)) uleft = true;

	// external corners
	if (up && left) ++corners;
	if (left && down) ++corners;
	if (down && right) ++corners;
	if (right && up) ++corners;

	//internal corners
	if (!right && !down && dright) ++corners;
	if (!right && !up && uright) ++corners;
	if (!left && !down && dleft) ++ corners;
	if (!left && !up && uleft) ++corners;
	return corners;
}

int getPerimeter(long x, long y, char last,std::vector<std::string>& board, std::vector<std::vector<bool>>& visited){
	if (!inBound(x,y,board)) return 0;
	if (!inBound(x,y,board)) return 0;
	if (board[static_cast<size_t>(x)][static_cast<size_t>(y)] != last) return 0;
	if (visited[static_cast<size_t>(x)][static_cast<size_t>(y)]) return 0;
	int ans{countSides(x,y,last,board)};
	visited[static_cast<size_t>(x)][static_cast<size_t>(y)] = true;
	ans += getPerimeter(x+1,y,last,board,visited);
	ans += getPerimeter(x-1,y,last,board,visited);
	ans += getPerimeter(x,y+1,last,board,visited);
	ans += getPerimeter(x,y-1,last,board,visited);
	return ans;
}

int getSides(long x, long y, char last,std::vector<std::string>& board, std::vector<std::vector<bool>>& visited){
	if (!inBound(x,y,board)) return 0;
	if (!inBound(x,y,board)) return 0;
	if (board[static_cast<size_t>(x)][static_cast<size_t>(y)] != last) return 0;
	if (visited[static_cast<size_t>(x)][static_cast<size_t>(y)]) return 0;
	int ans{countCorners(x,y,last,board)};
	visited[static_cast<size_t>(x)][static_cast<size_t>(y)] = true;
	ans += getSides(x+1,y,last,board,visited);
	ans += getSides(x-1,y,last,board,visited);
	ans += getSides(x,y+1,last,board,visited);
	ans += getSides(x,y-1,last,board,visited);
	return ans;
}

void sol1(){
	std::vector<std::string> lines;
	parseFile("12/input.txt", lines);
	std::vector<std::vector<bool>> visitedForArea(lines.size(), std::vector<bool>(lines[0].size(), false));
	std::vector<std::vector<bool>> visitedForPerimeter(lines.size(), std::vector<bool>(lines[0].size(), false));
	int totalPrice{0};
	for(long x{0}; x < static_cast<long>(lines.size()); ++x){
		for(long y{0}; y < static_cast<long>(lines[0].size()); ++y){
			const char currChar{lines[static_cast<size_t>(x)][static_cast<size_t>(y)]};
			totalPrice += getArea(x,y,currChar,lines,visitedForArea) * getPerimeter(x,y,currChar,lines,visitedForPerimeter);
		}
	}
	std::cout << "total price: " << totalPrice;
}

void sol2(){
	std::vector<std::string> lines;
	parseFile("12/input.txt", lines);
	std::vector<std::vector<bool>> visitedForArea(lines.size(), std::vector<bool>(lines[0].size(), false));
	std::vector<std::vector<bool>> visitedForPerimeter(lines.size(), std::vector<bool>(lines[0].size(), false));
	int totalPrice{0};
	for(long x{0}; x < static_cast<long>(lines.size()); ++x){
		for(long y{0}; y < static_cast<long>(lines[0].size()); ++y){
			const char currChar{lines[static_cast<size_t>(x)][static_cast<size_t>(y)]};
			totalPrice += getArea(x,y,currChar,lines,visitedForArea) * getSides(x,y,currChar,lines,visitedForPerimeter);
		}
	}
	std::cout << "total price: " << totalPrice;	
}

int main() {
	sol2();
}