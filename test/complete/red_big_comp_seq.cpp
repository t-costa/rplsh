//reduce(comp(seq, seq, seq))
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  source_vecpair_stage source;
  drain_stage drain;
  map_vecpair_vecpair_stage a;
  map_vecpair_vec_stage b;
  reduce_vec_double_stage c;

  utils::write("source_vecpair_stage, map_vecpair_vecpair_stage, map_vecpair_vec_stage, reduce_vec_double_stage, drain_stage", "./res_seq.txt");

  while (source.has_next()) {
    auto v = source.next();
    *v = a.compute(*v);
    auto vb = new std::vector<utils::elem_type>(b.compute(*v));
    auto res = new utils::elem_type(c.compute(*vb));
    drain.process(res);
  }

  utils::write("\n---------------------\n", "./res_seq.txt");

  return 0;
}
