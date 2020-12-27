//seq with datap
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  map_vec_vec_stage a;
  source_vec_stage source;
  drain_vec_stage drain;

  while (source.has_next()) {
    auto vs = source.next();
    *vs = a.compute(*vs);
    // std::vector<utils::elem_type> *va = new std::vector<utils::elem_type>();
    // *va = a.compute(*vs);
    drain.process(vs);
  }

  return 0;
}
