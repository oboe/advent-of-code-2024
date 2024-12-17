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
#include <cmath>
#include <exception>

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

enum class Instruction{
	ADV,
	BXL,
	BST,
	JNZ,
	BXC,
	OUT,
	BDV,
	CDV
};

struct Register {
	long long a{};
	long long b{};
	long long c{};
};

Instruction getInstruction(int i){
	return static_cast<Instruction>(i);
}

std::ostream & operator << (std::ostream & outs, const Instruction & instruction) {
	switch (instruction)
	{
	case Instruction::ADV:
		return outs << "adv(div on a with combo > a)\n";
	case Instruction::BXL:
		return outs << "bxl(xor on b and literal > b)\n";
	case Instruction::BST:
		return outs << "bst(mod8 on combo > b)\n";
	case Instruction::JNZ:
		return outs << "jnz(jump if not zero)\n";
	case Instruction::BXC:
		return outs << "bxc(xor on b and c > b)\n";
	case Instruction::OUT:
		return outs << "out(mod8 on combo > output)\n";
	case Instruction::BDV:
		return outs << "bdv(div on b with combo > b)\n";
	case Instruction::CDV:
		return outs << "cdv(div on c with combo > c)\n";
	default:
		return outs;
	}
}

std::ostream & operator << (std::ostream & outs, const Register & registers) {
	outs << "Registers(" << registers.a << "," << registers.b << "," << registers.c <<")\n";
	return outs;
}

void parseFile(std::string filePath, Register& registers, std::vector<int>& actions){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string line{};
	bool readingRegisters{true};
	int registerCount{0};
	while (std::getline(inFile, line)) {
		if(line.empty()) {
			readingRegisters = false;
			continue;
		}
		if (readingRegisters){
			std::vector<int> val{parseNumbers<int>(line)};
			if (registerCount == 0) {
				registers.a = val.front();
			} else if (registerCount == 1) {
				registers.b = val.front();
			} else {
				registers.c = val.front();
			}
			++registerCount;
		} else {
			actions = parseNumbers<int>(line);
		}
	}
	inFile.close();
}

int getLiteralOp(const int opPointer, const std::vector<int>& actions){
	return actions[static_cast<size_t>(opPointer)]; 
}

long long getComboOp(const int opPointer, const Register& registers, const std::vector<int>& actions){
	int op{actions[static_cast<size_t>(opPointer)]};
	if(op <= 3) {
		return op;
	} else if (op == 4) {
		return registers.a;
	} else if (op == 5) {
		return registers.b;
	} else if (op == 6) {
		return registers.c;
	} else {
		throw std::out_of_range("op code larger than 6");
	}
}

void run(Instruction instruction, int& instructionPointer, Register& registers, std::vector<int>& actions, std::vector<int>& output) {
	if (instruction == Instruction::ADV) {
		registers.a = (registers.a)/static_cast<int>(std::pow(2,getComboOp(instructionPointer+1,registers,actions)));
	} else if (instruction == Instruction::BXL) {
		registers.b = registers.b ^ getLiteralOp(instructionPointer + 1, actions);
	} else if (instruction == Instruction::BST) {
		registers.b = getComboOp(instructionPointer +1, registers,actions) % 8;
	} else if (instruction == Instruction::JNZ) {
		if (registers.a != 0){
			instructionPointer = getLiteralOp(instructionPointer + 1, actions);
			return;
		}
	} else if (instruction == Instruction::BXC) {
		registers.b = registers.b ^ registers.c;
	} else if (instruction == Instruction::OUT) {
		output.push_back(getComboOp(instructionPointer + 1, registers, actions) % 8);
	} else if (instruction == Instruction::BDV) {
		registers.b = (registers.a)/static_cast<int>(std::pow(2,getComboOp(instructionPointer+1,registers,actions))); 
	} else { // CDV
		registers.c = (registers.a)/static_cast<int>(std::pow(2,getComboOp(instructionPointer+1,registers,actions))); 
	}
	instructionPointer += 2;
}

std::vector<int> runProgram(Register& registers, std::vector<int>& actions, bool earlyExit = false){
	int instructionPointer{0};
	std::vector<int> output{};
	while(actions.size() > 0){
		if (instructionPointer >= static_cast<int>(actions.size())) break;
		Instruction instruction {getInstruction(actions[static_cast<size_t>(instructionPointer)])};
		//std::cout << instruction << registers;
		run(instruction, instructionPointer ,registers, actions, output);
		if (earlyExit) {
			for(size_t i = 0 ; i < output.size(); ++i){
				if (output[i] != actions[i]) return output;
			}
		}
	}
	return output;
}

void sol1(){
	std::vector<int> actions{};
	Register registers{};
	parseFile("17/input.txt", registers, actions);
	std::cout << registers;
	for(int i : actions){
		std::cout << i << " ";
	}
	std::cout << "\n";
	std::vector<int> output{runProgram(registers, actions)};
	for(int i : output){
		std::cout << i << ",";
	}
	std::cout << "\n";
}

void check(long long val, std::vector<int> actions){
	Register curr{val,0,0};
	std::vector<int> output{runProgram(curr, actions)};
	std::cout << "check: ";
	for(int i : output) {
		std::cout << i << ",";
	}
	std::cout << "\n";
}

bool checkSatisfaction(const long long a, const long long prev, const long long x){
	if (a/8 != prev) return false;
	const long long modA{(a%8) xor 2};
	const long long denominator{static_cast<long long>(std::pow(2,modA))};
	const long long l{modA xor (a/(denominator)) xor 3};
	return ((l%8) == x);
}

void sol2(){
	std::vector<int> actions{};
	Register registers{};
	parseFile("17/input.txt", registers, actions);
	std::vector<int> defaultActions = actions;
	//check(9, defaultActions);
	std::unordered_set<long long> aSet{};
	aSet.insert(0);
	std::reverse(actions.begin(), actions.end());
	for(int i : actions){
		std::unordered_set<long long> newASet{};
		for(long long a: aSet){
			long long newA{a*8};
			for(long long j{0}; j < 8 ; ++j){
				if (checkSatisfaction(newA,a,i)) newASet.insert(newA);
				++newA;
			}
			check(a, defaultActions);
		}
		aSet = newASet;
	}
	for(long long l :aSet){
		std::cout << "ans: " << l << "\n";
		check(l,defaultActions);
	}
}

int main() {
	sol2();
}