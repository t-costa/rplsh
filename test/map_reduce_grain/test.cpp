// pipe(source_unbalanced_vec_stage,mc,fm,r,drain_stage)

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
#include </home/tommaso/forked/rplsh/test/definition.hpp>


class source_unbalanced_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_unbalanced_vec_stage> src;

public:
	source_unbalanced_vec_stage_stage() : src(new source_unbalanced_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_unbalanced_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
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

class map0_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_minus_stage wrapper0;
	map_plus_stage wrapper1;
public:
	map0_stage() : ff_Map(1) {
		pfr.disableScheduler(0);
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		pfr.parallel_for(0, _task.size(), 1, 200, [this, &_task, &out](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			(*out)[i] = wrapper1.op(res0);
		},1);
		return out;
	}
};

class map1_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_plus_stage wrapper0;
public:
	map1_stage() : ff_Map(1) {
		pfr.disableScheduler(0);
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		pfr.parallel_for_static(0, _task.size(), 1, 0, [this, &_task, &out](const long i) {
			(*out)[i] = wrapper0.op(_task[i]);
		},1);
		return out;
	}
};

class reduce0_stage : public ff_Map<std::vector<utils::elem_type>,utils::elem_type,utils::elem_type> {
protected:
	reduce_stage wrapper0;
public:
	reduce0_stage() : ff_Map(1) {
		pfr.disableScheduler(1);
	}

	utils::elem_type* svc(std::vector<utils::elem_type>* t) {
		std::vector<utils::elem_type>& _task = *t;
		utils::elem_type* out  = new utils::elem_type(wrapper0.identity);
		auto reduceF = [this](utils::elem_type& sum, utils::elem_type elem) {sum = wrapper0.op(sum, elem);};
		auto bodyF = [this,&_task](const long i, utils::elem_type& sum) {sum = wrapper0.op(sum, _task[i]);};
		pfr.parallel_reduce_static(*out, wrapper0.identity, 0, _task.size(), 1, 0, bodyF, reduceF, 1);

		delete t;
		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2,3,4,5,6,7,8,9,10";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_unbalanced_vec_stage_stage _source_unbalanced_vec_stage;
	map0_stage _map0_;

	// vector of workers of farm
	std::vector<ff_node*> workers;

	// farm's worker 1
	map1_stage _map1_;
	workers.push_back(&_map1_);

	// add workers to farm
	ff_farm farm;
	farm.add_workers(workers);
	farm.add_collector(NULL);

	reduce0_stage _red0_;
	drain_stage_stage _drain_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_unbalanced_vec_stage);
	pipe.add_stage(&_map0_);
	pipe.add_stage(&farm);
	pipe.add_stage(&_red0_);
	pipe.add_stage(&_drain_stage);


	parameters::sequential = false;
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}
