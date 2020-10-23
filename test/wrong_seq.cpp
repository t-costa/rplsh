//map(farm(a))
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  map_vecpair_vec_stage a;
  source_vecpair_stage source;
  drain_vec_stage drain;

  while (source.has_next()) {
    auto vs = source.next();
    auto va = a.compute(v);
    drain.process(va);
  }

  return 0;
}
