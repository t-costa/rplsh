// pipe(source_vecpair_stage,reduce(comp(map_vecpair_vecpair_stage,map_vecpair_vec_stage,reduce_vec_double_stage)) with [ nw: 1],drain_stage)

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

class reduce0_stage : public ff_Map<utils::vec_pair,utils::elem_type> {
protected:
	map_vecpair_vecpair_stage wrapper0;
	map_vecpair_vec_stage wrapper1;
	reduce_vec_double_stage wrapper2;
public:
	reduce0_stage() : ff_Map(1) {
	}

	utils::elem_type* svc(utils::vec_pair* t) {
		utils::vec_pair& _task = *t;
		utils::elem_type* out  = new utils::elem_type(wrapper2.identity);
		auto reduceF = [this](utils::elem_type& sum, utils::elem_type elem) {sum = wrapper2.op(sum, elem);};
		std::vector<utils::elem_type>* mapout = new std::vector<utils::elem_type>();
		mapout->resize(_task.size());
		pfr.parallel_for_static(0, _task.size(), 1, 0, [this, &_task, &mapout](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			(*mapout)[i] = wrapper1.op(res0);
		},1);
		auto bodyF = [this,&mapout](const long i, utils::elem_type& sum) {sum = wrapper2.op(sum, (*mapout)[i]);};
		pfr.parallel_reduce_static(*out, wrapper2.identity, 0, mapout->size(), 1, 0, bodyF, reduceF, 1);

		delete mapout;

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_vecpair_stage_stage _source_vecpair_stage;
	reduce0_stage _red0_;
	drain_stage_stage _drain_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_vecpair_stage);
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
