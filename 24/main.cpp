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
  while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
    end = str.find(delim, start);
    std::string word{str.substr(start, end - start)};
    ans.push_back(fromString<T>(word));
  }
  return ans;
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

enum class Op { AND, OR, XOR, NONE };
enum class Tag { IN, CARRY, CARRY_HOLD, OUT, ANDN, ORN, XORN, NONE };

struct Gate {
  std::string name;
  std::string gate1;
  std::string gate2;
  Op op;
  std::optional<bool> value;
  Tag tag{Tag::NONE};
  std::string num{""};
};

std::string toStr(const Op& op) {
  if (op == Op::AND) return "AND";
  if (op == Op::OR) return "OR";
  if (op == Op::XOR) return "XOR";
  if (op == Op::NONE) return "NONE";
  return "";
}

std::ostream& operator<<(std::ostream& outs, const Op& op) {
  return outs << toStr(op);
}

std::string toStr(const Tag tag) {
  if (tag == Tag::IN) return "IN";
  if (tag == Tag::CARRY) return "CARRY";
  if (tag == Tag::CARRY_HOLD) return "CARRY_HOLD";
  if (tag == Tag::OUT) return "OUT";
  if (tag == Tag::ANDN) return "ANDN";
  if (tag == Tag::ORN) return "ORN";
  if (tag == Tag::XORN) return "XORN";
  return "";
}

std::string toStr(const Gate gate) {
  if (gate.tag == Tag::NONE)
    return gate.num + toStr(gate.tag) + "[" + gate.name + "](" + gate.gate1 +
           " " + toStr(gate.op) + " " + gate.gate2 + ") ===== NO TAG! =====\n";
  return gate.num + toStr(gate.tag) + "[" + gate.name + "](" + gate.gate1 +
         " " + toStr(gate.op) + " " + gate.gate2 + ")\n";
}

std::ostream& operator<<(std::ostream& outs, const Tag& tag) {
  return outs << toStr(tag);
}

std::ostream& operator<<(std::ostream& outs, const Gate& gate) {
  return outs << toStr(gate);
}

void parseFile(std::string filePath,
               std::unordered_map<std::string, Gate>& wireMp) {
  std::ifstream inFile;
  inFile.open(filePath);
  if (!inFile.is_open()) {
    std::cerr << "Could not open " << filePath << "\n";
    exit(1);
  }
  std::string line{};
  bool notOnGates{true};
  while (std::getline(inFile, line)) {
    if (line.empty()) {
      notOnGates = false;
      continue;
    }
    if (notOnGates) {
      bool val{(line[5] == '1') ? true : false};
      wireMp[line.substr(0, 3)] = {line.substr(0, 3), "", "", Op::NONE, val};
    } else {
      std::vector<std::string> words{split<std::string>(line)};
      std::string a{words[0]}, b{words[2]}, op{words[1]}, out{words[4]};
      if (op == "AND") {
        wireMp[out] = {out, a, b, Op::AND, {}};
      } else if (op == "OR") {
        wireMp[out] = {out, a, b, Op::OR, {}};
      } else {
        wireMp[out] = {out, a, b, Op::XOR, {}};
      }
    }
  }
  inFile.close();
}

bool calcWire(std::string wire, std::unordered_map<std::string, Gate>& wireMp,
              std::unordered_set<std::string>& visited) {
  // check value
  if (wireMp.count(wire) > 0 && wireMp[wire].value.has_value()) {
    return wireMp[wire].value.value();
  }
  if (visited.count(wire) > 0)
    throw std::invalid_argument("already visited wire: " + wire);
  visited.insert(wire);
  // recursively compute input wires to calc value
  const bool a{calcWire(wireMp[wire].gate1, wireMp, visited)};
  const bool b{calcWire(wireMp[wire].gate2, wireMp, visited)};
  bool ans{};

  if (wireMp[wire].op == Op::AND)
    ans = a && b;
  else if (wireMp[wire].op == Op::OR)
    ans = a || b;
  else if (wireMp[wire].op == Op::XOR)
    ans = a ^ b;

  // cache value
  wireMp[wire].value = ans;
  // return
  return ans;
}

void sol1() {
  std::unordered_map<std::string, Gate> wireMp{};
  parseFile("24/input.txt", wireMp);
  // pass through every z mp
  std::vector<std::pair<std::string, bool>> finalAns{};
  std::unordered_set<std::string> visited{};
  for (auto [k, v] : wireMp) {
    if (k[0] == 'z') {
      bool ans{calcWire(k, wireMp, visited)};
      finalAns.emplace_back(k, ans);
    }
  }
  for (auto [k, v] : wireMp) {
    std::cout << k << " " << v;
  }
  sort(finalAns.begin(), finalAns.end());
  long long ans{};
  for (size_t i{0}; i < finalAns.size(); ++i) {
    std::cout << finalAns[i].first << ": " << finalAns[i].second << "\n";
    ans += static_cast<long long>(finalAns[i].second)
           << static_cast<long long>(i);
  }
  std::cout << "final answer: " << ans;
  assert(ans == 42049478636360);
}

int getDigits(const std::string s) {
  std::stringstream ss(s);
  int code;
  ss >> code;
  return code;
}

std::pair<long long, long long> calcXY(
    std::unordered_map<std::string, Gate>& wireMp) {
  std::vector<std::pair<std::string, bool>> xAns{};
  std::vector<std::pair<std::string, bool>> yAns{};
  for (auto [k, v] : wireMp) {
    if (k[0] == 'x') {
      xAns.emplace_back(k, v.value.value());
    } else if (k[0] == 'y') {
      yAns.emplace_back(k, v.value.value());
    }
  }
  sort(xAns.begin(), xAns.end());
  sort(yAns.begin(), yAns.end());
  long long x{};
  long long y{};
  for (size_t i{0}; i < xAns.size(); ++i) {
    x += static_cast<long long>(xAns[i].second) << static_cast<long long>(i);
    y += static_cast<long long>(yAns[i].second) << static_cast<long long>(i);
  }
  return {x, y};
}

bool checkWires(std::unordered_map<std::string, Gate>& wireMp, long long x,
                long long y) {
  std::vector<std::pair<std::string, bool>> zAns{};
  const long long xy{x + y};
  std::unordered_set<std::string> visited{};
  for (auto [k, v] : wireMp) {
    if (k[0] == 'z') {
      bool ans{calcWire(k, wireMp, visited)};
      zAns.emplace_back(k, ans);
    }
  }
  sort(zAns.begin(), zAns.end());
  long long z{};
  for (size_t i{0}; i < zAns.size(); ++i) {
    z += static_cast<long long>(zAns[i].second) << static_cast<long long>(i);
  }
  if (xy == z) {
    std::cout << x << " + " << y << " = " << z << ", actual: " << x + y
              << " CORRECT\n";
    return true;
  } else {
    return false;
  }
}

void populateTags(std::string wire,
                  std::unordered_map<std::string, Gate>& wireMp,
                  std::unordered_set<std::string>& visited) {
  if (visited.count(wire) > 0 || wire == "") return;
  visited.insert(wire);
  populateTags(wireMp[wire].gate1, wireMp, visited);
  populateTags(wireMp[wire].gate2, wireMp, visited);
  const Gate a{wireMp[wireMp[wire].gate1]};
  const Gate b{wireMp[wireMp[wire].gate2]};
  if (wire[0] == 'y' || wire[0] == 'x') {
    wireMp[wire].tag = Tag::IN;
    wireMp[wire].num = wire.substr(1, 2);
  } else if (a.tag == Tag::IN && b.tag == Tag::IN && a.num == b.num) {
    if (wireMp[wire].op == Op::AND) {
      wireMp[wire].tag = Tag::ANDN;
      if (a.name == "y00") wireMp[wire].tag = Tag::CARRY;
    } else if (wireMp[wire].op == Op::OR) {
      wireMp[wire].tag = Tag::ORN;
    } else {
      wireMp[wire].tag = Tag::XORN;
    }
    wireMp[wire].num = a.num;
  } else if (wireMp[wire].op == Op::XOR && a.tag == Tag::XORN &&
             b.tag == Tag::CARRY) {
    wireMp[wire].tag = Tag::OUT;
    wireMp[wire].num = a.num;
  } else if (wireMp[wire].op == Op::XOR && b.tag == Tag::XORN &&
             a.tag == Tag::CARRY) {
    wireMp[wire].tag = Tag::OUT;
    wireMp[wire].num = b.num;
  } else if (wireMp[wire].op == Op::AND && a.tag == Tag::XORN &&
             b.tag == Tag::CARRY) {
    wireMp[wire].tag = Tag::CARRY_HOLD;
    wireMp[wire].num = a.num;
  } else if (wireMp[wire].op == Op::AND && b.tag == Tag::XORN &&
             a.tag == Tag::CARRY) {
    wireMp[wire].tag = Tag::CARRY_HOLD;
    wireMp[wire].num = b.num;
  } else if (wireMp[wire].op == Op::OR && a.tag == Tag::ANDN &&
             b.tag == Tag::CARRY_HOLD) {
    wireMp[wire].tag = Tag::CARRY;
    wireMp[wire].num = a.num;
  } else if (wireMp[wire].op == Op::OR && b.tag == Tag::ANDN &&
             a.tag == Tag::CARRY_HOLD) {
    wireMp[wire].tag = Tag::CARRY;
    wireMp[wire].num = a.num;
  } else {
    wireMp[wire].num = a.num;
  }
  return;
}

void sol2() {
  std::unordered_map<std::string, Gate> wireMp{};
  parseFile("24/input.txt", wireMp);
  std::vector<std::pair<std::string, std::string>> swaps{
      {"gws", "nnt"}, {"z13", "npf"}, {"z19", "cph"}, {"hgj", "z33"}};
  std::vector<std::string> ans{};
  for (auto [a, b] : swaps) {
    Gate temp{wireMp[a]};
    wireMp[a] = wireMp[b];
    wireMp[b] = temp;
    ans.push_back(a);
    ans.push_back(b);
  }
  sort(ans.begin(), ans.end());
  std::unordered_set<std::string> visited{};
  for (auto [k, v] : wireMp) {
    populateTags(k, wireMp, visited);
  }
  std::vector<std::string> vv{};
  for (auto [k, v] : wireMp) {
    if (v.op == Op::NONE) continue;
    // std::cout << v;
    vv.emplace_back(toStr(v));
  }
  sort(vv.begin(), vv.end());
  for (auto a : vv) {
    std::cout << a;
  }
  std::cout << "ans: ";
  for (auto s : ans) {
    std::cout << s << ",";
  }
}

int main() {
  // sol1();
  sol2();
}

// 08[z09](pcd XOR gws) ===== NO TAG! ===== swap gws and nnt
// 09ANDN[gws](x09 AND y09)
// 09XORN[nnt](y09 XOR x09)