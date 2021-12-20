#include "EarleyParser.h"

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

  std::vector<std::vector<Configuration> >
      configurations(word.size() + 1, std::vector<Configuration>());

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
