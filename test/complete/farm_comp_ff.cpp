// pipe(source_vecpair_stage,farm(comp(map_vecpair_vecpair_stage,map_vecpair_vec_stage)) with [ nw: 1],drain_vec_stage)

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
#include </home/tommaso/forked/rplsh/test/complete/definition.hpp>


class source_vecpair_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_vecpair_stage> src; 

public:
	source_vecpair_stage_stage() : src(new source_vecpair_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_vecpair_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
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

class map_vecpair_vecpair_stage_stage : public ff_node {
protected:
	map_vecpair_vecpair_stage wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- map_vecpair_vecpair_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		utils::vec_pair _in  = *((utils::vec_pair*) t);
		utils::vec_pair* _out  = new utils::vec_pair();
		*_out = wrapper.compute(_in);
		delete ((utils::vec_pair*) t);
		return (void*) _out;
	}
};

class map_vecpair_vec_stage_stage : public ff_node {
protected:
	map_vecpair_vec_stage wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- map_vecpair_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		utils::vec_pair _in  = *((utils::vec_pair*) t);
		std::vector<utils::elem_type>* _out  = new std::vector<utils::elem_type>();
		*_out = wrapper.compute(_in);
		delete ((utils::vec_pair*) t);
		return (void*) _out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_vecpair_stage_stage _source_vecpair_stage;
	
	// vector of workers of farm
	std::vector<ff_node*> workers;
	
	// farm's worker 1
	map_vecpair_vecpair_stage_stage _map_vecpair_vecpair_stage;
	map_vecpair_vec_stage_stage _map_vecpair_vec_stage;
	ff_comp comp;
	comp.add_stage(&_map_vecpair_vecpair_stage);
	comp.add_stage(&_map_vecpair_vec_stage);
	
	workers.push_back(&comp);
	
	// add workers to farm
	ff_farm farm;
	farm.add_workers(workers);
	farm.add_collector(NULL);
	
	drain_vec_stage_stage _drain_vec_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_vecpair_stage);
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
