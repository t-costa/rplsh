//seq standard
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  seq_vec_vec_stage a;
  source_vec_stage source;
  drain_vec_stage drain;

  utils::write("source_vec_stage, seq_vec_vec_stage, drain_vec_stage", "./res_seq.txt");

  while (source.has_next()) {
    auto vs = source.next();
    *vs = a.compute(*vs);
    drain.process(vs);
  }

  utils::write("\n---------------------\n", "./res_seq.txt");

  return 0;
}
