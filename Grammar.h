#pragma once

#include <utility>
#include <vector>
#include <istream>
#include <unordered_set>
#include <unordered_map>

struct Rule {
  char from;
  std::vector<char> to;

  Rule() = default;
  Rule(size_t from, const std::vector<char> &to) : from(from), to(to) {};

  static const char epsilon = '\0';
};

bool operator==(const Rule &l, const Rule &r);

class Grammar {
 public:

  Grammar() = default;
  Grammar(const Grammar &g) = default;
  Grammar(const std::unordered_set<char> &non_terminals,
          const std::unordered_set<char> &terminals,
          const std::unordered_map<char, std::vector<Rule>> &rules, char start)
      : non_terminals(non_terminals),
        terminals(terminals),
        rules(rules),
        start(start) {};

  void AddNonTerminal(const char c) { non_terminals.insert(c); }
  void AddTerminal(const char c) { terminals.insert(c); }
  void AddRule(const Rule &rule) { rules[rule.from].push_back(rule); }
  void SetStart(char start);

  const std::unordered_set<char> &GetNonTerminals() const;
  const std::unordered_set<char> &GetTerminals() const;
  const std::unordered_map<char, std::vector<Rule>> &GetRules() const;
  const std::vector<Rule> &GetRulesByChar(char c) const;
  char GetStart() const;

  bool IsNonTerminal(char c) const;
  bool IsTerminal(char c) const;
  bool IsEpsilonNonTerminal(char c) const;

  void PrecalcEpsilonNonTerminals();

 private:
  std::unordered_set<char> non_terminals;
  std::unordered_set<char> terminals;
  std::unordered_map<char, std::vector<Rule>> rules;
  std::unordered_set<char> epsilon_non_terminals;

  char start;

};

std::istream &operator>>(std::istream &in, Grammar &g);

template<>
struct std::hash<Rule> {
  std::size_t operator()(const Rule &rule) const noexcept {
    std::string s(rule.to.begin(), rule.to.end());
    s += rule.from;
    return std::hash<std::string>{}(s);
  }
};
