#include <sstream>
#include <queue>
#include "Grammar.h"

std::istream &operator>>(std::istream &in, Rule &rule) {

  char c;
  in >> c;

  rule.from = c;

  in >> c >> c;

  std::string right_part_string;
  getline(in, right_part_string);
  std::stringstream right_part(right_part_string);

  rule.to.clear();

  while (right_part >> c) {
    rule.to.push_back(c);
  }

  return in;
}

std::istream &operator>>(std::istream &in, Grammar &g) {

  size_t n, m, p;
  in >> n >> m >> p;

  g = Grammar();

  char c;
  for (size_t i = 0; i < n; ++i) {
    in >> c;
    g.AddNonTerminal(c);
  }

  for (size_t i = 0; i < m; ++i) {
    in >> c;
    g.AddTerminal(c);
  }

  Rule rule;
  for (size_t i = 0; i < p; ++i) {
    in >> rule;
    g.AddRule(rule);
  }

  in >> c;
  g.SetStart(c);

  return in;
}

bool operator==(const Rule &l, const Rule &r) {
  return l.from == r.from && l.to == r.to;
}

const std::unordered_set<char> &Grammar::GetNonTerminals() const {
  return non_terminals;
}
const std::unordered_set<char> &Grammar::GetTerminals() const {
  return terminals;
}

const std::unordered_map<char, std::vector<Rule>> &Grammar::GetRules() const {
  return rules;
}

bool Grammar::IsNonTerminal(char c) const {
  return non_terminals.count(c) > 0;
}
const std::vector<Rule> &Grammar::GetRulesByChar(char c) const {
  return rules.at(c);
}
bool Grammar::IsTerminal(char c) const {
  return terminals.count(c) > 0;
}

char Grammar::GetStart() const {
  return start;
}
void Grammar::SetStart(char start) {
  Grammar::start = start;
}
void Grammar::PrecalcEpsilonNonTerminals() {
  epsilon_non_terminals.clear();

  std::queue<char> queue;
  for (const auto &c: non_terminals) {
    for (const auto &rule: rules[c]) {
      if (rule.to.empty()) {
        queue.push(c);
        break;
      }
    }
  }
  while (!queue.empty()) {
    char c = queue.front();
    queue.pop();
    epsilon_non_terminals.insert(c);
    for (const auto &d: non_terminals) {
      if (epsilon_non_terminals.count(d) > 0) continue;
      for (const auto &rule: rules[d]) {
        bool is_epsilon_gen = true;
        for (const auto &to: rule.to) {
          if (epsilon_non_terminals.count(to) <= 0) {
            is_epsilon_gen = false;
            break;
          }
        }
        if (is_epsilon_gen) {
          epsilon_non_terminals.insert(d);
          queue.push(d);
          break;
        }
      }
    }
  }
}

bool Grammar::IsEpsilonNonTerminal(char c) const {
  return epsilon_non_terminals.count(c) > 0;
}
