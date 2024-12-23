#include <algorithm>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

void parseFile(std::string filePath,
               std::vector<std::pair<std::string, std::string>>& connections) {
  std::ifstream inFile;
  inFile.open(filePath);
  if (!inFile.is_open()) {
    std::cerr << "Could not open " << filePath << "\n";
    exit(1);
  }
  std::string line{};
  while (std::getline(inFile, line)) {
    connections.emplace_back(line.substr(0, 2), line.substr(3, 2));
  }
  inFile.close();
}

std::string getMpStr(std::string a, std::string b, std::string c) {
  std::vector<std::string> v{a, b, c};
  sort(v.begin(), v.end());
  return v[0] + v[1] + v[2];
}

void sol1() {
  std::vector<std::pair<std::string, std::string>> connections{};
  parseFile("23/input.txt", connections);
  std::unordered_map<std::string, std::unordered_set<std::string>> mp{};
  for (auto [a, b] : connections) {
    mp[a].insert(b);
    mp[b].insert(a);
  }
  std::vector<std::vector<std::string>> triplets{};
  std::unordered_set<std::string> tripletSeen{};
  for (auto [first, firstAdjacents] : mp) {
    for (auto second : firstAdjacents) {
      if (second == first) continue;
      for (auto third : mp[second]) {
        if (third == second || third == first) continue;
        if (mp[third].count(first) == 0) continue;
        std::string mpStr{getMpStr(first, second, third)};
        if (tripletSeen.count(mpStr) > 0) continue;
        tripletSeen.insert(mpStr);
        triplets.push_back({first, second, third});
      }
    }
  }
  std::vector<std::vector<std::string>> tripletsWithT{};
  for (auto v : triplets) {
    bool hasT{false};
    for (auto s : v) {
      if (s[0] == 't') hasT = true;
    }
    if (hasT) {
      tripletsWithT.push_back(v);
    }
  }
  std::cout << "triples with t: " << tripletsWithT.size();
  assert(tripletsWithT.size() == 1083);
}

std::string getMpStr(std::vector<std::string> v) {
  sort(v.begin(), v.end());
  std::string ans{""};
  for (auto s : v) {
    ans += s;
  }
  return ans;
}

std::vector<std::vector<std::string>> getFullyConnected(
    std::unordered_map<std::string, std::unordered_set<std::string>>& mp,
    std::unordered_set<std::string>& seen, std::vector<std::string> curr) {
  std::vector<std::vector<std::string>> ans{};
  if (seen.count(getMpStr(curr)) > 0) return ans;
  for (std::string s : curr) {  // for every node, check all adjacents to see if
                                // we can add something to it.
    for (std::string ss : mp[s]) {
      if (std::find(curr.begin(), curr.end(), ss) != curr.end()) continue;
      // check if this new node can be fully connected to it
      // so every element in the existing graph has a connection
      bool fullyConnected{true};
      for (std::string sss : curr) {
        if (sss == s) continue;
        if (mp[ss].count(sss) == 0) {
          fullyConnected = false;
        }
      }
      if (fullyConnected) {
        std::vector<std::string> newCurr{curr};
        seen.insert(getMpStr(newCurr));
        newCurr.push_back(ss);
        ans.push_back(newCurr);
      }
    }
  }
  return ans;
}

void sol2() {
  std::vector<std::pair<std::string, std::string>> connections{};
  parseFile("23/input.txt", connections);
  std::unordered_map<std::string, std::unordered_set<std::string>> mp{};
  for (auto [a, b] : connections) {
    mp[a].insert(b);
    mp[b].insert(a);
  }
  std::unordered_set<std::string> seen{};
  std::vector<std::vector<std::string>> fullConnected{};
  for (auto [k, v] : mp) {
    std::vector<std::vector<std::string>> tempConnected{
        getFullyConnected(mp, seen, {k})};
    fullConnected.insert(fullConnected.begin(), tempConnected.begin(),
                         tempConnected.end());
  }
  std::vector<std::string> best{};
  while (fullConnected.size() > 0) {
    std::vector<std::vector<std::string>> newFullConnected{};
    for (auto vv : fullConnected) {
      std::vector<std::vector<std::string>> tempConnected{
          getFullyConnected(mp, seen, vv)};
      if (tempConnected.size() > 0)
        newFullConnected.insert(newFullConnected.end(), tempConnected.begin(),
                                tempConnected.end());
    }
    if (newFullConnected.size() > 0) {
      std::vector<std::string> tt = *newFullConnected.begin();
      if (best.size() < tt.size()) best = tt;
    }
    fullConnected = newFullConnected;
  }
  sort(best.begin(), best.end());
  for (auto vv : best) {
    std::cout << vv << ",";
  }
  std::cout << "\n";
}

int main() {
  // sol1();
  sol2();
}