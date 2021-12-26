#include <iostream>
#include <fstream>
#include "Grammar.h"
#include "EarleyParser.h"

int main() {

  for (size_t j = 1; j <= 14; ++j) {
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
    }
    bool passed = true;
    for (size_t i = 0; i < m; ++i) {
      in >> word;
      if (word != answers[i]) {
        passed = false;
        std::cerr << "Error!" << std::endl;
      }
    }
    if(passed) {
      std::cout << "Test" << j << " passed!" << std::endl;
    }
  }
  return 0;
}
