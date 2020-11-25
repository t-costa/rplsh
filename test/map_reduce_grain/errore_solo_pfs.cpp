// pipe(source_vec_stage,m2,drain_vec_stage)

#include <iostream>
#include <vector>

// specify include directory for fastflow
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include <ff/map.hpp>
#include <ff/pipeline.hpp>
#include <ff/parallel_for.hpp>

// specify include directory for RPL-Shell
// #include <aux/types.hpp>
// #include <aux/wrappers.hpp>
// #include <aux/ff_comp.hpp>

// business code headers
// #include </home/tommaso/forked/rplsh/test/map_reduce_grain/seq_definition.hpp>

struct map_plus_stage {
public:
  explicit map_plus_stage() {}

  std::vector<int> compute(std::vector<int>& in) {
    std::vector<int> out;
    out.reserve(in.size());

    for (auto& el: in) {
      out.emplace_back(op(el));
    }

    return out;
  }

  int op(const int& in) {
    return in+2;
  }
};

// class map0_stage : public ff_Map<std::vector<int>,std::vector<int>> {
// protected:
// 	map_plus_stage wrapper0;
// public:
// 	map0_stage() : ff_Map(4) {
// 		pfr.disableScheduler(0);
// 	}
//
// 	std::vector<int>* svc(std::vector<int> *t) {
// 		std::vector<int>& _task = *t;
// 		std::vector<int>* out = &_task;
// 		pfr.parallel_for_static(0, _task.size(), 1, -100, [this, &_task, &out](const long i) {
// 			(*out)[i] = wrapper0.op(_task[i]);
// 		},4);
// 		return out;
// 	}
// };

int main( int argc, char* argv[] ) {
	// worker mapping
	// const char worker_mapping[] = "0,1,2,3,4,5,6,7";
	// threadMapper::instance()->setMappingList(worker_mapping);
	// source_vec_stage_stage _source_vec_stage;
	//map0_stage _map0_;
  ff::ParallelFor pfr;
  map_plus_stage m;
  std::vector<int>_task;
  auto* out = new std::vector<int>();
  out->resize(1000);
  for (auto i=0; i<1000; ++i){
    _task.push_back(i);
  }
  for (int i=1; i<100; ++i) {
    pfr.parallel_for_static(0, _task.size(), 1, -50, [&m, &_task, &out](const long i) {
      (*out)[i] = m.op(_task[i]);
    },4);
    std::cout << "iterazione: " << i << std::endl;
  }

	// drain_vec_stage_stage _drain_vec_stage;
	// ff_pipeline pipe;
	// pipe.add_stage(&_source_vec_stage);
//	pipe.add_stage(&_map0_);
	// pipe.add_stage(&_drain_vec_stage);


  //numeri da 2 a 101
  // for (auto el : *out) {
  //   std::cout << el << std::endl;
  // }

	// pipe.run_and_wait_end();
	// std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	return 0;

}
