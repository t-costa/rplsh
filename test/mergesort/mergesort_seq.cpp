#include <iostream>
#include <vector>
#include <functional>

#include "/home/tommaso/forked/rplsh/test/definition.hpp"
 // #include "/home/costanzo/rplsh/test/definition.hpp"

inline double avg(double a, double b) {
  if (a == 0) return b;
  return static_cast<double>((a+b)/2);
}

int main() {
  source_range_stage s;
  drain_range_stage d;
  dc_mergesort m;

  auto start = std::chrono::high_resolution_clock::now();
  double avg_source = 0, avg_m = 0, avg_drain = 0;

  while (s.has_next()) {
    auto start_source = std::chrono::high_resolution_clock::now();
    auto mm = s.next();
    auto end_source = std::chrono::high_resolution_clock::now();
    auto elapsed_source = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_source-start_source).count());
    // source_times.emplace_back(elapsed_source);
    avg_source = avg(avg_source, elapsed_source);

    auto start_m = std::chrono::high_resolution_clock::now();
    *mm = m.compute(*mm);
    auto end_m = std::chrono::high_resolution_clock::now();
    auto elapsed_m = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_m-start_m).count());
    // m_times.emplace_back(elapsed_m);
    avg_m = avg(avg_m, elapsed_m);

    auto start_drain = std::chrono::high_resolution_clock::now();
    d.process(mm);
    auto end_drain = std::chrono::high_resolution_clock::now();
    auto elapsed_drain = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_drain-start_drain).count());
    // drain_times.emplace_back(elapsed_drain);
    avg_drain = avg(avg_drain, elapsed_drain);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "Total time: " << duration.count() << " milliseconds" << std::endl;

  std::cout << "Average source: " << avg_source << " nanoseconds" << std::endl;
  std::cout << "Average dc: " << avg_m << " nanoseconds" << std::endl;
  std::cout << "Average drain: " << avg_drain << " nanoseconds" << std::endl;


  return 0;
}
