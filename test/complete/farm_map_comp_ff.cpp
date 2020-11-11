// pipe(source_vecpair_stage,farm(map(comp(map_vecpair_vec_stage,map_vec_vec_stage)) with [ nw: 1]) with [ nw: 1],drain_vec_stage)

#include <iostream>
#include <vector>

// specify include directory for fastflow
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include <ff/map.hpp>
#include <ff/pipeline.hpp>
#include <ff/parallel_for.hpp>

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

class map0_stage : public ff_Map<utils::vec_pair,std::vector<utils::elem_type>> {
protected:
	map_vecpair_vec_stage wrapper0;
	map_vec_vec_stage wrapper1;
public:
	map0_stage() : ff_Map(1) {
		pfr.disableScheduler(0);
	}

	std::vector<utils::elem_type>* svc(utils::vec_pair *t) {
		utils::vec_pair& _task = *t;
		std::vector<utils::elem_type>* out = new std::vector<utils::elem_type>();
		out->resize(_task.size());
		ff_Map<utils::vec_pair,std::vector<utils::elem_type>>::parallel_for(0, _task.size(),[this, &_task, &out](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			(*out)[i] = wrapper1.op(res0);
		},1);
		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5,6";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_vecpair_stage_stage _source_vecpair_stage;
	
	// vector of workers of farm
	std::vector<ff_node*> workers;
	
	// farm's worker 1
	map0_stage _map0_;
	workers.push_back(&_map0_);
	
	// add workers to farm
	ff_farm farm;
	farm.add_workers(workers);
	farm.add_collector(NULL);
	
	drain_vec_stage_stage _drain_vec_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_vecpair_stage);
	pipe.add_stage(&farm);
	pipe.add_stage(&_drain_vec_stage);
	
	
	parameters::sequential = false;
	utils::write("pipe(source_vecpair_stage,farm(map(comp(map_vecpair_vec_stage,map_vec_vec_stage)) with [ nw: 1]) with [ nw: 1],drain_vec_stage)", "./res_ff.txt");
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	utils::write("\n---------------------\n", "./res_ff.txt");
	return 0;
	
}
