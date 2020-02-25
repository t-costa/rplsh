#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include "sequential.hpp"
#include "parallel.hpp"


template <typename T>
void print(std::string const& s, std::vector<T> const& v) {
    std::cout << s << ": ";
    for (const auto& el : v) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
}

template <typename T>
void print(std::string const& s, T const& n) {
    std::cout << s << ": " << n << std::endl;
}

template <typename T>
bool check(std::vector<T> const& v1, std::vector<T> const& v2) {
    if (v1.size() != v2.size()) return false;

    for (size_t i=0; i<v1.size(); ++i) {
        if (v1[i] != v2[i]) return false;
    }

    return true;
}

int main() {
    std::cout << "insert seed, size and dimension" << std::endl;
    uint seed;
    size_t size;
    int dimension;
    std::cin >> seed >> size >> dimension;

    srand (seed);

    generator g(size, dimension);
    sorter s;
    multiplier m;
    sum p;
    std::vector<double> final_results;
    std::vector<double> ff_final_results;

    auto start = std::chrono::system_clock::now();
    while (g.has_next()) {
        auto v = g.next();
        *v = s.compute(*v);
        *v = m.compute(*v);
        auto res = p.compute(*v);
        //final_results.push_back(res);
        delete (v);
    }
    auto end = std::chrono::system_clock::now();


    //std::sort(final_results.begin(), final_results.end());

    //print("All final results", final_results);

    std::cout << "Execution time: " <<
    std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

    //calls parallel
    // const char worker_mapping[] = "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15";
    // threadMapper::instance()->setMappingList(worker_mapping);
    const char worker_mapping[] = "0,1,2,3,4,5,6,7";
    threadMapper::instance()->setMappingList(worker_mapping);

    srand (seed);

    generator_stage _generator(size, dimension);

    // vector of workers of farm
    std::vector<ff_node*> workers;
    // farm's worker 1
    sorter_stage _sorter;
    workers.push_back(&_sorter);
    // farm's worker 2
    // sorter_stage _sorter1;
    // workers.push_back(&_sorter1);
    // // farm's worker 3
    // sorter_stage _sorter2;
    // workers.push_back(&_sorter2);
    // // farm's worker 4
    // sorter_stage _sorter3;
    // workers.push_back(&_sorter3);
    // // farm's worker 5
    // sorter_stage _sorter4;
    // workers.push_back(&_sorter4);
    // farm's worker 6
    // sorter_stage _sorter5;
    // workers.push_back(&_sorter5);

    // add workers to farm
    ff_farm<> farm;
    farm.add_workers(workers);
    farm.add_collector(NULL);

    map0_stage _map0_;
    reduce0_stage _red0_;
    ff_pipeline pipe;
    pipe.add_stage(&_generator);
    pipe.add_stage(&farm);
    pipe.add_stage(&_map0_);
    pipe.add_stage(&_red0_);


    //pipe.create_output_buffer(100);
    pipe.run_and_wait_end();
    // pipe.run();
    //
    // int i=0;
    // double* ff_res;
    // while (i < dimension) {
    //   pipe.load_result((void**) &ff_res);
    //   ff_final_results.push_back(*ff_res);
    //   i++;
    // }
    //
    // pipe.wait();


    // auto output = pipe.get_out_buffer();
    //
    // for (const auto& el : output) {
    //   std::cout << el << " ";
    // }
    // std::cout << std::endl;

    // auto* ff_p = _red0_.get_out_buffer();
    // int i=0;
    // while (i < dimension) {
    //   double* data;
    //   std::cout << i << " ";
    //   i++;
    //   (*ff_p).pop((void**)&data);
    //   print("data",(*data));
    //   ff_final_results.push_back((*data));
    // }

    // for (int i=0; i<dimension; ++i) {
    //   ff_final_results.push_back(*(ff_p+i));
    // }
    //std::sort(ff_final_results.begin(), ff_final_results.end());

    //print("[FF] All final results", ff_final_results);

    std::cout << "Execution time:" << pipe.ffTime() << " ms" << std::endl;

    #ifdef TRACE_FASTFLOW
    std::cout << "Stats:" << std::endl;
    pipe.ffStats(std::cout);
    #endif
    return 0;
}
