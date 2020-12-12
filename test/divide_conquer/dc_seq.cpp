//fibonacci
#include <iostream>
#include <vector>

#include "definition.hpp"

int main() {
  dc_double_double_stage a;
  source_stage source;
  drain_stage drain;

  utils::write("source_stage, dc_double_double_stage, drain_stage", "./res_seq.txt");

  while (source.has_next()) {
    auto vs = source.next();
    *vs = a.compute(*vs);
    drain.process(vs);
  }

  utils::write("\n---------------------\n", "./res_seq.txt");

  return 0;
}
