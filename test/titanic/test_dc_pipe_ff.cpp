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

int i = 0;


class source_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_vec_stage> src;

public:
	source_vec_stage_stage() : src(new source_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<drain_vec_stage> drn;

public:
	drain_vec_stage_stage() : drn(new drain_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((std::vector<utils::elem_type>*) t);
		i = 0;
		return (GO_ON);
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2";
	threadMapper::instance()->setMappingList(worker_mapping);
	int nw = 1;

	 while (nw <= 10) {
	 	source_vec_stage_stage _source_vec_stage;
	 	dc_dummy dc_stage;
	 	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_(
	 		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
	 			dc_stage.divide(in, in_vec);
	 		},
	 		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
	 			dc_stage.combine(out_vec, out);
	 		},
	 		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
	 			dc_stage.seq(in, out);
	 		},
	 		[&](const std::vector<utils::elem_type>& in) {
	 			//i++;
	 			//std::cout << i << std::endl;
	 			return dc_stage.cond(in);
	 		},
	 	nw);
	 	drain_vec_stage_stage _drain_vec_stage;
	 	ff_pipeline pipe;
	 	pipe.add_stage(&_source_vec_stage);
	 	pipe.add_stage(&_dc0_);
	 	pipe.add_stage(&_drain_vec_stage);
	
	 	pipe.run_and_wait_end();
	 	std::cout << "Divide and conquer: nw = " << nw << ". ";
	 	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	 	if (nw < 32)
	 		nw++;
	 	else
			nw *= 2;
	 }

	//while (nw <= 128) {
	//	source_vec_stage _source_vec_stage;
	//	auto start = _source_vec_stage.next();
	//	//utils::print_vec(*start);
	//	auto end = std::vector<utils::elem_type>(parameters::inputsize);
	//	dc_dummy dc_stage;
	//	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_(
	//		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
	//			dc_stage.divide(in, in_vec);
	//		},
	//		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
	//			dc_stage.combine(out_vec, out);
	//		},
	//		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
	//			dc_stage.seq(in, out);
	//		},
	//		[&](const std::vector<utils::elem_type>& in) {
				//i++;
				//std::cout << i << std::endl;
	//			return dc_stage.cond(in);
	//		}, *start, end, nw);
		// drain_vec_stage_stage _drain_vec_stage;
		// ff_pipeline pipe;
	//	 pipe.add_stage(&_source_vec_stage);
	//	 pipe.add_stage(&_dc0_);
	//	 pipe.add_stage(&_drain_vec_stage);
		
	//	 pipe.run_and_wait_end();
		//ffTime(START_TIME);
		//_dc0_.run_and_wait_end();
		//ffTime(STOP_TIME);

	//	std::cout << "Divide and conquer: nw = " << nw << ". ";
		//utils::print_vec(end);
		//std::cout << "Spent: " << ffTime(GET_TIME) << " usec" << std::endl;
	//	 std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	//	if (nw < 32)
	//		nw++;
	//	else
	//		nw *= 2;
//	}



	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}
