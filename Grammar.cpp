#include <sstream>
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
  std::unordered_set<char> used;
  dfs(start, used);
}

bool Grammar::dfs(char v, std::unordered_set<char> &used) {
  used.insert(v);
  bool flag = false;
  for (const auto &rule : rules[v]) {
    for (const auto &c: rule.to) {
      if (IsNonTerminal(c) && used.count(c) <= 0) {
        if (dfs(c, used)) {
          flag = true;
        }
      }
    }
    if (rule.to.empty()) {
      flag = true;
    }
  }
  if (flag) {
    epsilon_non_terminals.insert(v);
  }
  return flag;
}

bool Grammar::IsEpsilonNonTerminal(char c) const {
  return epsilon_non_terminals.count(c) > 0;
}
