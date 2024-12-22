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
std::vector<T> parseNumbers(const std::string& line) {
  std::vector<T> ans;
  std::regex pattern("-?\\d+");
  auto words_begin = std::sregex_iterator(line.begin(), line.end(), pattern);
  auto words_end = std::sregex_iterator();
  for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
    std::string match = (*i).str();
    ans.push_back(fromString<T>(match));
  }
  return ans;
}

void parseFile(std::string filePath, std::vector<int64_t>& startingNumbers) {
  std::ifstream inFile;
  inFile.open(filePath);
  if (!inFile.is_open()) {
    std::cerr << "Could not open " << filePath << "\n";
    exit(1);
  }
  std::string line{};
  while (std::getline(inFile, line)) {
    startingNumbers.push_back(parseNumbers<int>(line)[0]);
  }
  inFile.close();
}

int64_t calc(std::vector<int64_t>& prices, int64_t i,
             const int64_t steps = 2000) {
  if (steps == 0) {
    prices.push_back(i);
    return i;
  }
  constexpr int64_t prune{16777216};
  int64_t a{calc(prices, i, steps - 1)};
  a = (a ^ (a * 64)) % prune;
  a = (a ^ (a / 32)) % prune;
  a = (a ^ (a * 2048)) % prune;
  prices.push_back(a);
  return a;
}

void sol1() {
  std::vector<int64_t> nums{};
  parseFile("22/input.txt", nums);
  int64_t sum{0};
  for (auto i : nums) {
    std::vector<int64_t> prices{};
    int64_t val{calc(prices, i)};
    sum += val;
  }
  assert(sum == 17724064040);
  std::cout << "sol1 sum: " << sum << "\n";
}

void pruneNumbers(std::vector<int64_t>& v) {
  for (size_t i{0}; i < static_cast<size_t>(v.size()); ++i) {
    std::string s{std::to_string(v[i])};
    v[i] = stoi(std::string{s[s.size() - 1]});
  }
}

int64_t buyBananas(const std::vector<int64_t>& prices,
                   const std::vector<int64_t>& command) {
  for (size_t i{4}; i < static_cast<size_t>(prices.size()); ++i) {
    std::vector<int64_t> rawPrices{prices[i - 4], prices[i - 3], prices[i - 2],
                                   prices[i - 1], prices[i]};
    pruneNumbers(rawPrices);
    std::vector<int64_t> rawPriceDiffs{
        rawPrices[1] - rawPrices[0], rawPrices[2] - rawPrices[1],
        rawPrices[3] - rawPrices[2], rawPrices[4] - rawPrices[3]};
    if (command == rawPriceDiffs) {
      return rawPrices[4];
    }
  }
  return 0;
}

std::string getMpStr(const int64_t a, const int64_t b, const int64_t c,
                     const int64_t d) {
  return std::to_string(a) + "," + std::to_string(b) + "," + std::to_string(c) +
         "," + std::to_string(d);
}

void slidingWindowScan(const std::vector<int64_t>& prices,
                       std::unordered_map<std::string, int64_t>& countMp) {
  for (size_t i{4}; i < static_cast<size_t>(prices.size()); ++i) {
    std::vector<int64_t> rawPrices{prices[i - 4], prices[i - 3], prices[i - 2],
                                   prices[i - 1], prices[i]};
    pruneNumbers(rawPrices);
    std::vector<int64_t> rawPriceDiffs{
        rawPrices[1] - rawPrices[0], rawPrices[2] - rawPrices[1],
        rawPrices[3] - rawPrices[2], rawPrices[4] - rawPrices[3]};
    ++countMp[getMpStr(rawPriceDiffs[0], rawPriceDiffs[1], rawPriceDiffs[2],
                       rawPriceDiffs[3])];
  }
}

void sol2() {
  std::vector<int64_t> nums{};
  parseFile("22/input.txt", nums);
  std::vector<std::vector<int64_t>> allPrices{};
  for (auto i : nums) {
    std::vector<int64_t> prices{};
    calc(prices, i);
    allPrices.push_back(prices);
  }
  std::unordered_map<std::string, int64_t> countMp{};
  for (auto v : allPrices) {
    slidingWindowScan(v, countMp);
  }
  int64_t bestSum{-1};
  for (int64_t a{-9}; a <= 9; ++a) {
    for (int64_t b{-9}; b <= 9; ++b) {
      for (int64_t c{-9}; c <= 9; ++c) {
        for (int64_t d{-9}; d <= 9; ++d) {
          const std::string mpStr{getMpStr(a, b, c, d)};
          if (countMp.count(mpStr) == 0) continue;
          if (countMp[mpStr] <= 300) continue;
          int64_t sum{0};
          std::vector<int64_t> command{a, b, c, d};
          for (std::vector<int64_t> p : allPrices) {
            sum += buyBananas(p, command);
          }
          if (sum > bestSum) {
            bestSum = sum;
            std::cout << command[0] << "," << command[1] << "," << command[2]
                      << "," << command[3] << "=" << sum << "\n";
          }
        }
      }
    }
  }
  assert(bestSum == 1998);
  std::cout << "sol2 sum: " << bestSum << "\n";
}

int main() {
  // sol1();
  sol2();
}