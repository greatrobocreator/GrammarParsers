#include <set>
#include <vector>
#include <istream>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <queue>
#include <iostream>

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

class Parser {
 public:

  virtual Parser &Fit(const Grammar &_g);
  virtual bool Predict(const std::string &word) const { return false; };

 protected:

  Grammar g;
};

Parser &Parser::Fit(const Grammar &_g) {
  g = _g;
  return *this;
}

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

/// ( A -> a.b, i ), dot is on k-th position
struct Configuration {

  Configuration() = default;
  Configuration(const Rule &rule, size_t i, size_t k = 0)
      : rule(rule), i(i), k(k) {}

  char NextChar() const;

  [[nodiscard]] std::string ToString() const {
    std::string s(rule.to.begin(), rule.to.end());
    return "( " + std::to_string(rule.from) + " -> " + s.substr(0, k) + "."
        + s.substr(k) + ", " + std::to_string(i) + " )";
  }

  Rule rule;
  size_t i, k;
};

bool operator==(const Configuration &l, const Configuration &r);

class EarleyParser : Parser {
 public:

  EarleyParser &Fit(const Grammar &_g) override;
  bool Predict(const std::string &word) const override;

 private:

  /// ( successful, new_configuration )
  static std::vector<Configuration> Scan(const Configuration &conf, char c);
  static std::vector<Configuration> Predict(const Configuration &conf,
                                            size_t j,
                                            const Grammar &g);
  static std::vector<Configuration> Complete(const Configuration &conf,
                                             const std::unordered_set<
                                                 Configuration> &possible_parents);

  static void InsertConfigurations(std::vector<Configuration> &configurations,
                                   std::unordered_set<Configuration> &configurations_set,
                                   const std::vector<Configuration> &to_insert);

  static void InsertConfiguration(std::vector<Configuration> &configurations,
                                  std::unordered_set<Configuration> &configurations_set,
                                  const Configuration &to_insert);

  static const char fake_start = '\1';
};

template<>
struct std::hash<Configuration> {
  std::size_t operator()(const Configuration &conf) const noexcept {
    std::size_t h = std::hash<Rule>{}(conf.rule);
    size_t offset = (conf.i + conf.k) % std::numeric_limits<size_t>::digits;
    return (h << offset)
        ^ (h >> (std::numeric_limits<size_t>::digits - offset));
  }
};

bool operator==(const Configuration &l, const Configuration &r) {
  return l.i == r.i && l.k == r.k && l.rule == r.rule;
}

EarleyParser &EarleyParser::Fit(const Grammar &_g) {
  Parser::Fit(_g);
  g.PrecalcEpsilonNonTerminals();
  return *this;
}

bool EarleyParser::Predict(const std::string &word) const {

  /// D[i] sets of configurations from algorithm
  std::vector<std::vector<Configuration> >
      configurations(word.size() + 1, std::vector<Configuration>());

  /// Also D[i] sets but in hash table for fast contains-checking
  std::vector<std::unordered_set<Configuration> >
      configurations_set(word.size() + 1, std::unordered_set<Configuration>());

  configurations[0].emplace_back(Rule(fake_start, {g.GetStart()}), 0, 0);
  configurations_set[0].insert(configurations[0][0]);

  for (size_t i = 0; i <= word.size(); ++i) {
    for (size_t j = 0; j < configurations[i].size(); ++j) {
      const Configuration &conf = configurations[i][j];
      if (conf.NextChar() == Rule::epsilon) {
        InsertConfigurations(configurations[i],
                             configurations_set[i],
                             Complete(conf, configurations_set[conf.i]));
      } else if (g.IsNonTerminal(conf.NextChar())) {
        InsertConfigurations(configurations[i],
                             configurations_set[i],
                             Predict(conf, i, g));
      } else if (i < word.size() && g.IsTerminal(conf.NextChar())) {
        InsertConfigurations(configurations[i + 1],
                             configurations_set[i + 1],
                             Scan(conf, word[i]));
      }
    }
  }

  return configurations_set[word.size()].count(Configuration(Rule(fake_start,
                                                                  {g.GetStart()}),
                                                             0,
                                                             1));
}

std::vector<Configuration> EarleyParser::Scan(const Configuration &conf,
                                              char c) {
  if (conf.NextChar() != c) {
    return {};
  }
  Configuration new_conf(conf);
  new_conf.k++;
  return {new_conf};
}

std::vector<Configuration> EarleyParser::Predict(const Configuration &conf,
                                                 size_t j,
                                                 const Grammar &g) {
  if (!g.IsNonTerminal(conf.NextChar())) {
    return std::vector<Configuration>();
  }
  const std::vector<Rule> &rules = g.GetRulesByChar(conf.NextChar());

  std::vector<Configuration> new_configs;
  new_configs.reserve(rules.size());
  for (const auto &rule : rules) {
    new_configs.emplace_back(rule, j);
  }
  if (g.IsEpsilonNonTerminal(conf.NextChar())) {
    new_configs.emplace_back(conf.rule, conf.i, conf.k + 1);
  }
  return new_configs;
}

std::vector<Configuration> EarleyParser::Complete(
    const Configuration &conf,
    const std::unordered_set<
        Configuration> &possible_parents) {
  if (conf.NextChar() != Rule::epsilon || conf.rule.from == fake_start) {
    return std::vector<Configuration>();
  }
  std::vector<Configuration> new_configs;
  for (const auto &parent: possible_parents) {
    if (parent.NextChar() == conf.rule.from) {
      new_configs.emplace_back(parent.rule, parent.i, parent.k + 1);
    }
  }
  return new_configs;
}
void EarleyParser::InsertConfigurations(std::vector<Configuration> &configurations,
                                        std::unordered_set<
                                            Configuration> &configurations_set,
                                        const std::vector<Configuration> &to_insert) {
  for (const auto &conf: to_insert) {
    InsertConfiguration(configurations, configurations_set, conf);
  }
}
void EarleyParser::InsertConfiguration(std::vector<Configuration> &configurations,
                                       std::unordered_set<
                                           Configuration> &configurations_set,
                                       const Configuration &to_insert) {
  if (configurations_set.count(to_insert) > 0) {
    return;
  }
  configurations.push_back(to_insert);
  configurations_set.insert(to_insert);
}

char Configuration::NextChar() const {
  if (k >= rule.to.size()) {
    return Rule::epsilon;
  }
  return rule.to[k];
}

int main() {

  Grammar g;
  std::cin >> g;

  EarleyParser parser;
  parser.Fit(g);

  size_t m;
  std::cin >> m;

  std::vector<std::string> answers(m);

  std::string word;
  for (size_t i = 0; i < m; ++i) {
    std::cin >> word;
    std::cout << (parser.Predict(word) ? "YES" : "NO") << std::endl;
  }

  return 0;
}
