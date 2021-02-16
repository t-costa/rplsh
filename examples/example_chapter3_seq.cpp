//matrix mul
#include <iostream>
#include <vector>
#include <functional>

#include "/home/tommaso/forked/rplsh/examples/example_code.hpp"
 // #include "/home/costanzo/rplsh/examples/example_code.hpp"

// using time = std::chrono::high_resolution_clock::time_point;
// using clock = std::chrono::high_resolution_clock;
inline double avg(double a, double b) {
  if (a == 0) return b;
  return static_cast<double>((a+b)/2);
}

/*
set dimension with 1000
set inputsize with 200
*/

int main() {
  seq_vec_vec a;
  map_vec_vec b;
  map_minus c;
  dc_dummy d;
  reduce_vec_double e;
  source_vec source;
  drain_elem drain;

  auto start = std::chrono::high_resolution_clock::now();
  // time start_source, start_m, start_drain;
  // time end_source, end_m, end_drain;

  // std::vector<std::chrono::nanoseconds> source_times, m_times, drain_times;
  // std::vector<double> source_times, m_times, drain_times;
  double avg_source = 0, avg_a = 0, avg_b = 0,
          avg_c = 0, avg_d = 0, avg_e = 0, avg_drain = 0;


  while (source.has_next()) {
    auto start_source = std::chrono::high_resolution_clock::now();
    auto v = source.next();
    auto end_source = std::chrono::high_resolution_clock::now();
    auto elapsed_source = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_source-start_source).count());
    // source_times.emplace_back(elapsed_source);
    avg_source = avg(avg_source, elapsed_source);

    //map_vec_vec
    auto start_b = std::chrono::high_resolution_clock::now();
    *v = b.compute(*v);
    auto end_b = std::chrono::high_resolution_clock::now();
    auto elapsed_b = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_b-start_b).count());
    // m_times.emplace_back(elapsed_m);
    avg_b = avg(avg_b, elapsed_b);

    //di nuovo b, non servo che lo conto
    //map_vec_vec
    *v = b.compute(*v);

    //seq_vec_vec
    auto start_a = std::chrono::high_resolution_clock::now();
    *v = a.compute(*v);
    auto end_a = std::chrono::high_resolution_clock::now();
    auto elapsed_a = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_a-start_a).count());
    // m_times.emplace_back(elapsed_m);
    avg_a = avg(avg_a, elapsed_a);

    //dc_dummy
    auto start_d = std::chrono::high_resolution_clock::now();
    *v = d.compute(*v);
    auto end_d = std::chrono::high_resolution_clock::now();
    auto elapsed_d = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_d-start_d).count());
    // m_times.emplace_back(elapsed_m);
    avg_d = avg(avg_d, elapsed_d);

    //map_minus
    auto start_c = std::chrono::high_resolution_clock::now();
    *v = c.compute(*v);
    auto end_c = std::chrono::high_resolution_clock::now();
    auto elapsed_c = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_c-start_c).count());
    // m_times.emplace_back(elapsed_m);
    avg_c = avg(avg_c, elapsed_c);

    //reduce_vec_double
    auto start_e = std::chrono::high_resolution_clock::now();
    auto res = new elem_type(e.compute(*v));
    delete v;
    auto end_e = std::chrono::high_resolution_clock::now();
    auto elapsed_e = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_e-start_e).count());
    // m_times.emplace_back(elapsed_m);
    avg_e = avg(avg_e, elapsed_e);

    auto start_drain = std::chrono::high_resolution_clock::now();
    drain.process(res);
    auto end_drain = std::chrono::high_resolution_clock::now();
    auto elapsed_drain = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_drain-start_drain).count());
    // drain_times.emplace_back(elapsed_drain);
    avg_drain = avg(avg_drain, elapsed_drain);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
  std::cout << "Total time: " << duration.count() << " milliseconds" << std::endl;

  //provo a prendere le medie
  // auto total_source = std::accumulate(source_times.begin(), source_times.end(), 0);
  // auto total_m = std::accumulate(m_times.begin(), m_times.end(), 0);
  // auto total_drain = std::accumulate(drain_times.begin(), drain_times.end(), 0);
  // std::cout << "Average source: " << (total_source/source_times.size()) << " nanoseconds" << std::endl;
  // std::cout << "Average map: " << (total_m/m_times.size()) << " nanoseconds" << std::endl;
  // std::cout << "Average drain: " << (total_drain/drain_times.size()) << " nanoseconds" << std::endl;


  std::cout << "Average source: " << avg_source << " nanoseconds" << std::endl;
  std::cout << "Average map_vec_vec: " << avg_b << " nanoseconds" << std::endl;
  std::cout << "Average seq_vec_vec: " << avg_a << " nanoseconds" << std::endl;
  std::cout << "Average dc_dummy: " << avg_d << " nanoseconds" << std::endl;
  std::cout << "Average map_minus: " << avg_c << " nanoseconds" << std::endl;
  std::cout << "Average reduce_vec_double: " << avg_e << " nanoseconds" << std::endl;
  std::cout << "Average drain: " << avg_drain << " nanoseconds" << std::endl;


  return 0;
}
