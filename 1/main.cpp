#include <iostream>
#include <fstream>

void parseFile(std::string relativeFileName){
	std::ifstream inFile;
	inFile.open(relativeFileName);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << relativeFileName << "\n";
		exit(1);
	}
	std::string x{};
	while (inFile >> x) {
		std::cout << x << "\n";
	}
	inFile.close();
}

int main(){
	parseFile("1/input.txt");
	return 0;
}