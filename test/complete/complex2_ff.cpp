// pipe(source_vecpair_stage,complex_farm,complex_comp,farm(farm(complex_reduce) with [ nw: 1]) with [ nw: 1],drain_stage)

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
#include </home/tommaso/forked/rplsh/test/definition.hpp>


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

class seq_vec_vec_stage_stage : public ff_node {
protected:
	seq_vec_vec_stage wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- seq_vec_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
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

class map_vec_vec_stage_stage : public ff_node {
protected:
	map_vec_vec_stage wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- map_vec_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
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

class map0_stage : public ff_Map<utils::vec_pair,std::vector<utils::elem_type>> {
protected:
	map_vecpair_vecpair_stage wrapper0;
	map_vecpair_vec_stage wrapper1;
	map_vec_vec_stage wrapper2;
public:
	map0_stage() : ff_Map(1) {
	}

	std::vector<utils::elem_type>* svc(utils::vec_pair *t) {
		utils::vec_pair& _task = *t;
		std::vector<utils::elem_type>* out = new std::vector<utils::elem_type>();
		out->resize(_task.size());
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			auto res1 = wrapper1.op(res0);
			(*out)[i] = wrapper2.op(res1);
		},1);

		delete t;

		return out;
	}
};

class reduce0_stage : public ff_Map<std::vector<utils::elem_type>,utils::elem_type, utils::elem_type> {
protected:
	map_vec_vec_stage wrapper0;
	reduce_vec_double_stage wrapper1;
public:
	reduce0_stage() : ff_Map(1) {
	}

	utils::elem_type* svc(std::vector<utils::elem_type>* t) {
		std::vector<utils::elem_type>& _task = *t;
		utils::elem_type* out  = new utils::elem_type(wrapper1.identity);
		auto bodyF = [this,&_task](const long i, utils::elem_type& sum) {
			auto res0 = wrapper0.op(_task[i]);
			sum = wrapper1.op(sum, res0);
		};
		auto reduceF = [this](utils::elem_type& sum, utils::elem_type elem) {sum = wrapper1.op(sum, elem);};
		pfr.parallel_reduce_static(*out, wrapper1.identity, 0, _task.size(), 1, 0, bodyF, reduceF, 1);

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14";
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
	farm.add_collector(nullptr);
	
	seq_vec_vec_stage_stage _seq_vec_vec_stage;
	map_vec_vec_stage_stage _map_vec_vec_stage;
	ff_comb comp(_seq_vec_vec_stage, _map_vec_vec_stage);
	
	// vector of workers of farm1
	std::vector<ff_node*> workers1;
	
	// farm1's worker 1
	
	// vector of workers of farm2
	std::vector<ff_node*> workers2;
	
	// farm2's worker 1
	reduce0_stage _red0_;
	workers2.push_back(&_red0_);
	
	// add workers2 to farm2
	ff_farm farm2;
	farm2.add_workers(workers2);
	farm2.add_collector(nullptr);
	
	workers1.push_back(&farm2);
	
	// add workers1 to farm1
	ff_farm farm1;
	farm1.add_workers(workers1);
	farm1.add_collector(nullptr);
	
	drain_stage_stage _drain_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_vecpair_stage);
	pipe.add_stage(&farm);
	pipe.add_stage(&comp);
	pipe.add_stage(&farm1);
	pipe.add_stage(&_drain_stage);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
