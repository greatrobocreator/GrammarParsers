#include "Parser.h"

Parser &Parser::Fit(const Grammar &_g) {
  g = _g;
  return *this;
}
