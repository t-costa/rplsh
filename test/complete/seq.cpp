//seq standard
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  seq_vec_vec_stage a;
  source_vec_stage source;
  drain_vec_stage drain;

  while (source.has_next()) {
    auto vs = source.next();
    *vs = a.compute(*vs);
    drain.process(vs);
  }

  return 0;
}
