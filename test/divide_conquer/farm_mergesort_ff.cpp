// pipe(source_range_stage,farm(divide_and_conquer(dc_mergesort) with [nw 1]) with [ nw: 1],drain_range_stage)

#include <iostream>
#include <vector>

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
#include </home/tommaso/forked/rplsh/test/divide_conquer/definition.hpp>


class source_range_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_range_stage> src; 

public:
	source_range_stage_stage() : src(new source_range_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_range_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_range_stage_stage : public ff_node {
protected:
	std::unique_ptr<drain_range_stage> drn; 

public:
	drain_range_stage_stage() : drn(new drain_range_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_range_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((utils::range*) t);
		return (GO_ON);
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_range_stage_stage _source_range_stage;
	
	// vector of workers of farm
	std::vector<ff_node*> workers;
	
	// farm's worker 1
	dc_mergesort dc_stage;
	ff_DC<utils::range, utils::range> _dc0_(
		[&](const utils::range& in, std::vector<utils::range>& in_vec) {
			dc_stage.divide(in, in_vec);
		},
		[&](std::vector<utils::range>& out_vec, utils::range& out) {
			dc_stage.combine(out_vec, out);
		},
		[&](const utils::range& in, utils::range& out) {
			dc_stage.seq(in, out);
		},
		[&](const utils::range& in) {
			return dc_stage.cond(in);
		},
	1);
	workers.push_back(&_dc0_);
	
	// add workers to farm
	ff_farm farm;
	farm.add_workers(workers);
	farm.add_collector(NULL);
	
	drain_range_stage_stage _drain_range_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_range_stage);
	pipe.add_stage(&farm);
	pipe.add_stage(&_drain_range_stage);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
