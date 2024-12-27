#include <algorithm>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

bool isKey(std::vector<std::string>& schema) {
  for (char c : schema[0]) {
    if (c != '.') return false;
  }
  return true;
}

std::vector<int> getHeights(std::vector<std::string>& schema) {
  const bool k{isKey(schema)};
  std::vector<int> ans{};
  for (size_t i{0}; i < schema[0].size(); ++i) {
    for (size_t height{0}; height < schema.size(); ++height) {
      if (k && schema[height][i] == '#') {
        ans.push_back(static_cast<int>(6 - height));
        break;
      }
      if (!k && schema[height][i] == '.') {
        ans.push_back(static_cast<int>(height - 1));
        break;
      }
    }
  }
  return ans;
}

void addSchema(std::vector<std::string>& schema,
               std::vector<std::vector<int>>& keys,
               std::vector<std::vector<int>>& locks) {
  const bool k{isKey(schema)};
  const std::vector<int> heights{getHeights(schema)};
  if (k) {
    keys.push_back(heights);
  } else {
    locks.push_back(heights);
  }
}

void parseFile(std::string filePath, std::vector<std::vector<int>>& keys,
               std::vector<std::vector<int>>& locks) {
  std::ifstream inFile;
  inFile.open(filePath);
  if (!inFile.is_open()) {
    std::cerr << "Could not open " << filePath << "\n";
    exit(1);
  }
  std::string line{};
  std::vector<std::string> schema{};
  while (std::getline(inFile, line)) {
    if (line.empty()) {
      addSchema(schema, keys, locks);
      schema.clear();
    } else {
      schema.push_back(line);
    }
  }
  addSchema(schema, keys, locks);
  inFile.close();
}

bool checkFit(std::vector<int>& key, std::vector<int>& lock) {
  for (size_t i{0}; i < key.size(); ++i) {
    if (lock[i] >= (6 - key[i])) return false;
  }
  return true;
}

void sol1() {
  std::vector<std::vector<int>> keys{};
  std::vector<std::vector<int>> locks{};
  parseFile("25/input.txt", keys, locks);
  std::cout << "==== keys ====\n";
  for (auto vv : keys) {
    for (auto i : vv) {
      std::cout << i << " ";
    }
    std::cout << "\n";
  }
  std::cout << "==== locks ====\n";
  for (auto vv : locks) {
    for (auto i : vv) {
      std::cout << i << " ";
    }
    std::cout << "\n";
  }
  int ans{0};
  for (auto key : keys) {
    for (auto lock : locks) {
      if (checkFit(key, lock)) ++ans;
    }
  }
  std::cout << "ans: " << ans;
}

void sol2() {}

int main() {
  sol1();
  // sol2();
}