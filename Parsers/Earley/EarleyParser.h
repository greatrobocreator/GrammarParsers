#pragma once

#include <set>
#include <utility>
#include "Parser.h"

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
