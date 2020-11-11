//reduce(seq, seq)
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  source_vec_stage source;
  drain_stage drain;
  map_vec_vec_stage a;
  reduce_vec_double_stage b;

  utils::write("source_vec_stage, map_vec_vec_stage, reduce_vec_double_stage, drain_stage", "./res_seq.txt");

  while (source.has_next()) {
    auto v = source.next();
    *v = a.compute(*v);
    auto res = new utils::elem_type(b.compute(*v));
    drain.process(res);
  }

  utils::write("\n---------------------\n", "./res_seq.txt");

  return 0;
}
