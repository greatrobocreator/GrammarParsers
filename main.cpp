#include <iostream>
#include "Grammar.h"
#include "EarleyParser.h"

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
