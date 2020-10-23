//farm(map(comp(seq, seq)))
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  map_vecpair_vec_stage a;
  map_vec_vec_stage b;
  source_vecpair_stage source;
  drain_vec_stage drain;

  while (source.has_next()) {
    auto vs = source.next();
    auto va = a.compute(v);
    auto vb = b.compute(vs);
    drain.process(vb);
  }

  return 0;
}
