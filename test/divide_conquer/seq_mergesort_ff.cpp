// pipe(source_range_stage,dc_mergesort,drain_range_stage)

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

class dc_mergesort_stage : public ff_node {
protected:
	dc_mergesort wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- dc_mergesort -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		utils::range _in  = *((utils::range*) t);
		utils::range* _out  = new utils::range();
		*_out = wrapper.compute(_in);
		delete ((utils::range*) t);
		return (void*) _out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_range_stage_stage _source_range_stage;
	dc_mergesort_stage _dc_mergesort;
	drain_range_stage_stage _drain_range_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_range_stage);
	pipe.add_stage(&_dc_mergesort);
	pipe.add_stage(&_drain_range_stage);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
