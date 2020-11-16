//comp(seq, seq)
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  map_vecpair_vecpair_stage a;
  map_vecpair_vec_stage b;
  source_vecpair_stage source;
  drain_vec_stage drain;

  utils::write("source_vecpair_stage, map_vecpair_vecpair_stage, map_vecpair_vec_stage, drain_vec_stage", "./res_seq.txt");

  while (source.has_next()) {
    utils::vec_pair* vs = source.next();
    (*vs) = a.compute(*vs);
    //auto va = a.compute(*vs);
    std::vector<utils::elem_type>* vb = new std::vector<utils::elem_type>();
    *vb = b.compute(*vs);
    drain.process(vb);
  }

  utils::write("\n---------------------\n", "./res_seq.txt");

  return 0;
}
