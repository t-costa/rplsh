// pipe(source_vec_stage,r,drain_stage)

#include <iostream>
#include <algorithm>
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
#include </home/costanzo/rplsh/test/definition.hpp>

int nw = 1;

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

class reduce0_stage : public ff_Map<std::vector<utils::elem_type>,utils::elem_type, utils::elem_type> {
protected:
	reduce_vec_double_stage wrapper0;
public:
	reduce0_stage() : ff_Map(nw) {
	}

	utils::elem_type* svc(std::vector<utils::elem_type>* t) {
		std::vector<utils::elem_type>& _task = *t;
		utils::elem_type* out  = new utils::elem_type(wrapper0.identity);
		auto reduceF = [this](utils::elem_type& sum, utils::elem_type elem) {sum = wrapper0.op(sum, elem);};
		auto bodyF = [this,&_task](const long i, utils::elem_type& sum) {sum = wrapper0.op(sum, _task[i]);};
		pfr.parallel_reduce_static(*out, wrapper0.identity, 0, _task.size(), 1, 0, bodyF, reduceF, nw);

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2,3,4";
	threadMapper::instance()->setMappingList(worker_mapping);


	while (nw <= 128) {
		source_vec_stage_stage _source_vec_stage;
		reduce0_stage _red0_;
		drain_stage_stage _drain_stage;
		ff_pipeline pipe;
		pipe.add_stage(&_source_vec_stage);
		pipe.add_stage(&_red0_);
		pipe.add_stage(&_drain_stage);
		pipe.run_and_wait_end();
		std::cout << "Reduce: nw = " << nw << ". ";
		std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
		if (nw < 32)
			nw++;
		else
			nw *= 2;
	}


	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}
