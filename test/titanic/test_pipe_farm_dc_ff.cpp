// pipe(source_vec_stage,fd,drain_vec_stage)

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
		return (GO_ON);
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_vec_stage_stage _source_vec_stage;
	
	// vector of workers of farm
	std::vector<ff_node*> workers;
	
	// farm's worker 1
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
			return dc_stage.cond(in);
		},
	16);
	workers.push_back(&_dc0_);
	
	// farm's worker 2
	dc_dummy dc_stage1;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_1(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage1.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage1.combine(out_vec, out);
		},
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			dc_stage1.seq(in, out);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return dc_stage1.cond(in);
		},
	16);
	workers.push_back(&_dc0_1);
	
	// farm's worker 3
	dc_dummy dc_stage2;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_2(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage2.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage2.combine(out_vec, out);
		},
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			dc_stage2.seq(in, out);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return dc_stage2.cond(in);
		},
	16);
	workers.push_back(&_dc0_2);
	
	// farm's worker 4
	dc_dummy dc_stage3;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_3(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage3.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage3.combine(out_vec, out);
		},
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			dc_stage3.seq(in, out);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return dc_stage3.cond(in);
		},
	16);
	workers.push_back(&_dc0_3);
	
	// farm's worker 5
	dc_dummy dc_stage4;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_4(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage4.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage4.combine(out_vec, out);
		},
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			dc_stage4.seq(in, out);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return dc_stage4.cond(in);
		},
	16);
	workers.push_back(&_dc0_4);
	
	// farm's worker 6
	dc_dummy dc_stage5;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_5(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage5.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage5.combine(out_vec, out);
		},
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			dc_stage5.seq(in, out);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return dc_stage5.cond(in);
		},
	16);
	workers.push_back(&_dc0_5);
	
	// farm's worker 7
	dc_dummy dc_stage6;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_6(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage6.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage6.combine(out_vec, out);
		},
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			dc_stage6.seq(in, out);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return dc_stage6.cond(in);
		},
	16);
	workers.push_back(&_dc0_6);
	
	// farm's worker 8
	dc_dummy dc_stage7;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_7(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage7.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage7.combine(out_vec, out);
		},
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			dc_stage7.seq(in, out);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return dc_stage7.cond(in);
		},
	16);
	workers.push_back(&_dc0_7);
	
	// add workers to farm
	ff_farm farm;
	farm.add_workers(workers);
	farm.add_collector(nullptr);
	drain_vec_stage_stage _drain_vec_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_vec_stage);
	pipe.add_stage(&farm);
	pipe.add_stage(&_drain_vec_stage);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
