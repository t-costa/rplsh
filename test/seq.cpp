//farm(map(seq))
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  map_vecpair_vec_stage a;
  source_vecpair_stage source;
  drain_vec_stage drain;

  while (source.has_next()) {
    auto vs = source.next();
    std::vector<double> *va = new std::vector<double>();
    *va = a.compute(*vs);
    drain.process(va);
  }

  return 0;
}
