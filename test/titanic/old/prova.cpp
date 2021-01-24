// pipe(source_vec_stage,d,drain_vec_stage)

#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

// specify include directory for fastflow
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include <ff/map.hpp>
#include <ff/pipeline.hpp>
#include <ff/parallel_for.hpp>
#include <ff/dc.hpp>

// specify include directory for RPL-Shell
#include <aux/types.hpp>
#include <aux/wrappers.hpp>
#include <aux/ff_comp.hpp>

// business code headers
#include </home/costanzo/rplsh/test/definition.hpp>

using namespace ff;
using namespace std;
using Problem = long;
using Result  = long;

class source_vec_stage_stage : public ff_node {
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
			int* a = new int(20);
			ff_send_out((void*) a);
		return (NULL);
	}
};

class drain_vec_stage_stage : public ff_node {
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		delete (int*)t;
		return (GO_ON);
	}
};




int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2";
	threadMapper::instance()->setMappingList(worker_mapping);
	int nw = 7;

	// while (nw <= 128) {
		source_vec_stage_stage _source_vec_stage;
		// dc_dummy dc_stage;
		// ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_(
		// 	[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
		// 		dc_stage.divide(in, in_vec);
		// 	},
		// 	[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
		// 		dc_stage.combine(out_vec, out);
		// 	},
		// 	[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
		// 		dc_stage.seq(in, out);
		// 	},
		// 	[&](const std::vector<utils::elem_type>& in) {
		// 		return dc_stage.cond(in);
		// 	},
		// nw);
		// long res;

		ff_DC<long, long> dac(
	                          [](const Problem &op,std::vector<Problem> &subops){
	                              subops.push_back(op-1);
	                              subops.push_back(op-2);
	                          },
	                          [](vector<Result>& res, Result &ret){ ret=res[0]+res[1]; },
	                          [](const Problem &, Result &res)  { res=1; },
	                          [](const Problem &op){ return (op<=2); },
	                          nw);
		drain_vec_stage_stage _drain_vec_stage;
		ff_pipeline pipe;
		pipe.add_stage(&_source_vec_stage);
		pipe.add_stage(&dac);
		pipe.add_stage(&_drain_vec_stage);

		pipe.run_and_wait_end();
		std::cout << "Divide and conquer: nw = " << nw << ". ";
		std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
		// if (nw < 32)
		// 	nw++;
		// else
		// 	nw *= 2;
	// }



	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}
