// pipe(source_vec_stage,f,drain_vec_stage)

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

class dc_dummy_stage : public ff_node {
protected:
	dc_dummy wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- dc_dummy -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		std::vector<utils::elem_type> _in  = *((std::vector<utils::elem_type>*) t);
		std::vector<utils::elem_type>* _out  = new std::vector<utils::elem_type>();
		*_out = wrapper.compute(_in);
		delete ((std::vector<utils::elem_type>*) t);
		return (void*) _out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_vec_stage_stage _source_vec_stage;
	
	// vector of workers of farm
	std::vector<ff_node*> workers;
	
	// farm's worker 1
	dc_dummy_stage _dc_dummy;
	workers.push_back(&_dc_dummy);
	
	// farm's worker 2
	dc_dummy_stage _dc_dummy1;
	workers.push_back(&_dc_dummy1);
	
	// farm's worker 3
	//dc_dummy_stage _dc_dummy2;
	//workers.push_back(&_dc_dummy2);
	
	// farm's worker 4
	//dc_dummy_stage _dc_dummy3;
	//workers.push_back(&_dc_dummy3);
	
	// farm's worker 5
	//dc_dummy_stage _dc_dummy4;
	//workers.push_back(&_dc_dummy4);
	
	// farm's worker 6
	//dc_dummy_stage _dc_dummy5;
	//workers.push_back(&_dc_dummy5);
	
	// farm's worker 7
	//dc_dummy_stage _dc_dummy6;
	//workers.push_back(&_dc_dummy6);
	
	// farm's worker 8
	//dc_dummy_stage _dc_dummy7;
	//workers.push_back(&_dc_dummy7);
	
	// farm's worker 9
	//dc_dummy_stage _dc_dummy8;
	//workers.push_back(&_dc_dummy8);
	
	// farm's worker 10
	//dc_dummy_stage _dc_dummy9;
	//workers.push_back(&_dc_dummy9);
	
	// farm's worker 11
	//dc_dummy_stage _dc_dummy10;
	//workers.push_back(&_dc_dummy10);
	
	// farm's worker 12
	//dc_dummy_stage _dc_dummy11;
	//workers.push_back(&_dc_dummy11);
	
	// farm's worker 13
	//dc_dummy_stage _dc_dummy12;
	//workers.push_back(&_dc_dummy12);
	
	// farm's worker 14
	//dc_dummy_stage _dc_dummy13;
	//workers.push_back(&_dc_dummy13);
	
	// farm's worker 15
	//dc_dummy_stage _dc_dummy14;
	//workers.push_back(&_dc_dummy14);
	
	// farm's worker 16
	//dc_dummy_stage _dc_dummy15;
	//workers.push_back(&_dc_dummy15);
	
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
