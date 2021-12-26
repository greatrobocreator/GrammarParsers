#pragma once

#include "../Grammar.h"

class Parser {
 public:

  virtual Parser &Fit(const Grammar &_g);
  virtual bool Predict(const std::string &word) const { return false; };

 protected:

  Grammar g;
};
