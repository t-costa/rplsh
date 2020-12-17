// pipe(source_vecpair_stage,complex_farm,complex_comp,complex_reduce,drain_stage)

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
		pfr.parallel_for_static(0, _task.size(), 1, 0, [this, &_task, &out](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			auto res1 = wrapper1.op(res0);
			(*out)[i] = wrapper2.op(res1);
		},1);

		delete t;

		return out;
	}
};

class reduce0_stage : public ff_Map<std::vector<utils::elem_type>,utils::elem_type> {
protected:
	map_vec_vec_stage wrapper0;
	reduce_vec_double_stage wrapper1;
public:
	reduce0_stage() : ff_Map(1) {
	}

	utils::elem_type* svc(std::vector<utils::elem_type>* t) {
		std::vector<utils::elem_type>& _task = *t;
		utils::elem_type* out  = new utils::elem_type(wrapper1.identity);
		auto reduceF = [this](utils::elem_type& sum, utils::elem_type elem) {sum = wrapper1.op(sum, elem);};
		std::vector<utils::elem_type>* mapout = new std::vector<utils::elem_type>();
		mapout->resize(_task.size());
		pfr.parallel_for_static(0, _task.size(), 1, 0, [this, &_task, &mapout](const long i) {
			(*mapout)[i] = wrapper0.op(_task[i]);
		},1);
		auto bodyF = [this,&mapout](const long i, utils::elem_type& sum) {sum = wrapper1.op(sum, (*mapout)[i]);};
		pfr.parallel_reduce_static(*out, wrapper1.identity, 0, mapout->size(), 1, 0, bodyF, reduceF, 1);

		delete mapout;

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5,6,7,8,9,10";
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
	
	seq_vec_vec_stage_stage _seq_vec_vec_stage;
	map_vec_vec_stage_stage _map_vec_vec_stage;
	ff_comp comp;
	comp.add_stage(&_seq_vec_vec_stage);
	comp.add_stage(&_map_vec_vec_stage);
	
	reduce0_stage _red0_;
	drain_stage_stage _drain_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_vecpair_stage);
	pipe.add_stage(&farm);
	pipe.add_stage(&comp);
	pipe.add_stage(&_red0_);
	pipe.add_stage(&_drain_stage);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
