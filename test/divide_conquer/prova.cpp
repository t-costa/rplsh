// pipe(source_stage,divide_and_conquer(dc_double_double_stage) with [nw 1],drain_stage)

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


class source_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_stage> src; 

public:
	source_stage_stage() : src(new source_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_stage_stage : public ff_node {
protected:
	std::unique_ptr<drain_stage> drn; 

public:
	drain_stage_stage() : drn(new drain_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((utils::elem_type*) t);
		return (GO_ON);
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_stage_stage _source_stage;
	dc_double_double_stage wrapper;
	ff_DC<utils::elem_type, utils::elem_type> _dc0_(
		[&](const utils::elem_type& in, std::vector<utils::elem_type>& in_vec) {
			wrapper.divide(in, in_vec);
		},
		[&](std::vector<utils::elem_type>& out_vec, utils::elem_type& out) {
			wrapper.combine(out_vec, out);
		},
		[&](const utils::elem_type& in, utils::elem_type& out) {
			wrapper.seq(in, out);
		},
		[&](const utils::elem_type& in) {
			return wrapper.cond(in);
		},
		1);
	drain_stage_stage _drain_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_stage);
	pipe.add_stage(&_dc0_);
	pipe.add_stage(&_drain_stage);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
