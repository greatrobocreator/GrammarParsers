#include <iostream>
#include <fstream>
#include "Grammar.h"
#include "EarleyParser.h"

int main() {

  for (size_t j = 1; j <= 14; ++j) {
    //freopen(("../Tests/Test" + std::to_string(j) + ".txt").c_str(), "r", stdin);
    std::ifstream in("../Tests/Test" + std::to_string(j) + ".txt");

    Grammar g;
    in >> g;

    EarleyParser parser;
    parser.Fit(g);

    size_t m;
    in >> m;

    std::vector<std::string> answers(m);

    std::string word;
    for (size_t i = 0; i < m; ++i) {
      in >> word;
      answers[i] = (parser.Predict(word) ? "YES" : "NO");
      //std::cout << (parser.Predict(word) ? "YES" : "NO") << std::endl;
    }
    for (size_t i = 0; i < m; ++i) {
      in >> word;
      if (word != answers[i]) {
        std::cerr << "Error!" << std::endl;
      }
    }
  }
  return 0;
}
