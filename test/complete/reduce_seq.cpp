//seq with reduce node
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {

  source_vec_stage source;
  drain_stage drain;
  reduce_vec_double_stage a;

  utils::write("source_vec_stage, reduce_vec_double_stage, drain_stage", "./res_seq.txt");

  while (source.has_next()) {
    auto v = source.next();
    auto res = new utils::elem_type(a.compute(*v));
    drain.process(res);
  }

  utils::write("\n---------------------\n", "./res_seq.txt");

  return 0;
}
