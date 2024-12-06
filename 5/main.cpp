#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <unordered_map>

template <class T>
std::vector<T> split(const std::string& str, const std::string delim = " ") {
	std::vector<T> ans;
	unsigned long start{};
	unsigned long end{};
	while ( (start = str.find_first_not_of(delim, end)) != std::string::npos ) {
		end = str.find(delim, start);
		std::string word{str.substr(start, end-start)};
		if constexpr (std::is_same<T, int>::value) {
			ans.push_back(T(stoi(word)));
		} else {
			ans.push_back(T(word));
		}
	}
	return ans;
}

void parseFile(std::string filePath, std::vector<std::pair<int,int>>& rules, std::vector<std::vector<int>>& updates){
	std::ifstream inFile;
	inFile.open(filePath);
	if (!inFile.is_open()) {
		std::cerr << "Could not open " << filePath << "\n";
		exit(1);
	}
	std::string x{};
	while (std::getline(inFile, x)) {
		if (x.find("|") != std::string::npos) {
			std::vector<int> ruleVals{split<int>(x,"|")};
			rules.push_back(std::make_pair(ruleVals[0], ruleVals[1]));
			continue;
		}
		if (x.find(",") != std::string::npos) {
			std::vector<int> update{split<int>(x,",")};
			updates.push_back(update);	
		}	
	}
	inFile.close();
}

bool isRightOrder(std::vector<int>& update, std::unordered_map<int, std::unordered_set<int>> breaching_rule_lookup) {
	std::unordered_set<int> seen;
	for(int i: update){
		for(int breachingNum : breaching_rule_lookup[i]){
			if(seen.count(breachingNum) > 0) {
				return false;
			}
		}
		seen.insert(i);
	}
	return true;
}

int getMiddleNumber(std::vector<int>& update) {
	return update[update.size()/2];
}

std::vector<int> reorder(std::vector<int>& update, std::unordered_map<int, std::unordered_set<int>> breaching_rule_lookup) {
	std::vector<int> remaining{update};
	std::vector<int> ans{};
	while(remaining.size() > 0){
		for(unsigned long i{0}; i < remaining.size(); ++i){
			std::unordered_set<int> must_come_before{breaching_rule_lookup[remaining[i]]};
			bool invalid{false};
			for(unsigned long j{0}; j < remaining.size(); ++j){
				if(i == j) {
					continue;
				}
				if( must_come_before.count(remaining[j]) > 0) {
					invalid = true;
				}
			}
			if (!invalid) {
				ans.push_back(remaining[i]);
				remaining.erase(remaining.begin() + static_cast<long>(i));
				break;
			}
		}
	}
	return ans;
}

void sol1() {
	std::vector<std::pair<int,int>> rules;
	std::vector<std::vector<int>> updates;
	parseFile("5/input.txt", rules, updates);
	std::unordered_map<int, std::unordered_set<int>> breaching_rule_lookup;
	for(std::pair<int,int> p : rules){
		breaching_rule_lookup[p.first].insert(p.second);
	}
	int count{0};
	for(std::vector<int> update: updates){
		if(isRightOrder(update, breaching_rule_lookup)) {
			count += getMiddleNumber(update);
		}
	}
	std::cout << "count: " << count;
}

void sol2() {
	std::vector<std::pair<int,int>> rules;
	std::vector<std::vector<int>> updates;
	parseFile("5/input.txt", rules, updates);
	std::unordered_map<int, std::unordered_set<int>> breaching_rule_lookup;
	std::unordered_map<int, std::unordered_set<int>> non_breaching_rule_lookup;
	for(std::pair<int,int> p : rules){
		breaching_rule_lookup[p.first].insert(p.second);
		non_breaching_rule_lookup[p.second].insert(p.first);
	}
	int count{0};
	for(std::vector<int> update: updates){
		if(!isRightOrder(update, breaching_rule_lookup)) {
			std::vector<int> reorderedUpdate{reorder(update, non_breaching_rule_lookup)};
			count += getMiddleNumber(reorderedUpdate);
		}
	}
	std::cout << "count: " << count;
}

int main(){
	sol2();
}
