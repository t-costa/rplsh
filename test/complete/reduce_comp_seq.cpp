//comp(seq_map, seq_red)
#include <iostream>
#include <vector>

#include "../definition.hpp"

int main() {
  source_vec_stage source;
  drain_stage drain;
  map_vec_vec_stage a;
  reduce_vec_double_stage b;

  while (source.has_next()) {
    auto v = source.next();
    *v = a.compute(*v);
    auto res = new utils::elem_type(b.compute(*v));
    drain.process(res);
  }

  return 0;
}
