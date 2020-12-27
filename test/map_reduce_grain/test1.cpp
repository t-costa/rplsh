#include <iostream>
#include <vector>

#include </home/tommaso/forked/rplsh/test/definition.hpp>

int main() {
  source_unbalanced_vec_stage s;
  map_minus_stage mm;
  map_plus_stage mp;
  reduce_stage r;
  drain_stage d;

  while (s.has_next()) {
    auto v = s.next();
    *v = mm.compute(*v);
    *v = mp.compute(*v);
    *v = mp.compute(*v);
    auto res = new utils::elem_type(r.compute(*v));
    delete v;
    d.process(res);
  }

  return 0;
}
