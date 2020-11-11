//pipe(
//  farm(
//    map(
//      comp(
//        map_vecpair_vecpair_stage, map_vecpair_vec_stage, map_vec_vec_stage
//      )
//    )
//  ),
//  comp(
//    seq_vec_vec_stage, map_vec_vec_stage
//  ),
//  reduce(
//    comp(
//      map_vec_vec_stage, reduce_vec_double_stage
//    )
//  )
//)


//comp(d, a, b, e, b, b, c)
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  map_vecpair_vec_stage a;
  map_vec_vec_stage b;
  reduce_vec_double_stage c;
  map_vecpair_vecpair_stage d;
  seq_vec_vec_stage e;
  source_vecpair_stage source;
  drain_stage drain;

  utils::write("source_vecpair_stage, map_vecpair_vecpair_stage, "
                "map_vecpair_vec_stage, map_vec_vec_stage, "
                "seq_vec_vec_stage, map_vec_vec_stage, "
                "map_vec_vec_stage, reduce_vec_double_stage, "
                "drain_stage", "./res_seq.txt");

  while (source.has_next()) {
    utils::vec_pair* vs = source.next();

    auto vd = d.compute(*vs);
    auto va = a.compute(vd);
    auto vb = b.compute(va);

    auto ve = e.compute(vb);
    vb = b.compute(ve);

    vb = b.compute(vb);
    utils::elem_type* vc = new utils::elem_type();
    *vc = c.compute(vb);

    drain.process(vc);
  }

  utils::write("\n---------------------\n", "./res_seq.txt");

  return 0;
}
