//seq with datap
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  map_vec_vec_stage a;
  source_vec_stage source;
  drain_vec_stage drain;

  utils::write("source_vec_stage, map_vec_vec_stage, drain_vec_stage", "./res_seq.txt");

  while (source.has_next()) {
    auto vs = source.next();
    *vs = a.compute(*vs);
    // std::vector<utils::elem_type> *va = new std::vector<utils::elem_type>();
    // *va = a.compute(*vs);
    drain.process(vs);
  }

  utils::write("\n---------------------\n", "./res_seq.txt");

  return 0;
}
