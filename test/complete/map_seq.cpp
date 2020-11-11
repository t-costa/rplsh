//seq with datap
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  map_vecpair_vec_stage a;
  source_vecpair_stage source;
  drain_vec_stage drain;

  utils::write("source_vecpair_stage, map_vecpair_vec_stage, drain_vec_stage", "./res_seq.txt");

  while (source.has_next()) {
    auto vs = source.next();
    std::vector<utils::elem_type> *va = new std::vector<utils::elem_type>();
    *va = a.compute(*vs);
    drain.process(va);
  }

  utils::write("\n---------------------\n", "./res_seq.txt");

  return 0;
}
