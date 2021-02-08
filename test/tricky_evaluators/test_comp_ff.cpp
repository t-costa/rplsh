// pipe(source_ordered_vec_stage,c,drain_ordered_vec_stage)

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
#include <ff/combine.hpp>

// specify include directory for RPL-Shell
#include <aux/types.hpp>
#include <aux/wrappers.hpp>
#include <aux/ff_comp.hpp>

// business code headers
#include </home/tommaso/forked/rplsh/test/definition.hpp>


class source_ordered_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_ordered_vec_stage> src;

public:
	source_ordered_vec_stage_stage() : src(new source_ordered_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_ordered_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_ordered_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<drain_ordered_vec_stage> drn;

public:
	drain_ordered_vec_stage_stage() : drn(new drain_ordered_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_ordered_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((std::vector<utils::elem_type>*) t);
		return (GO_ON);
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

class map0_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_vec_vec_stage wrapper0;
public:
	map0_stage() : ff_Map(2) {
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			(*out)[i] = wrapper0.op(_task[i]);
		},2);

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,1,2,3,4,5";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_ordered_vec_stage_stage _source_ordered_vec_stage;
	map_vec_vec_stage_stage _map_vec_vec_stage;
	map0_stage _map0_;
	ff_comb pipe1(&_map_vec_vec_stage,&_map0_ );
	// pipe1.add_stage(&_map_vec_vec_stage);
	// pipe1.add_stage(&_map0_);

	drain_ordered_vec_stage_stage _drain_ordered_vec_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_ordered_vec_stage);
	pipe.add_stage(&pipe1);
	pipe.add_stage(&_drain_ordered_vec_stage);


	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}