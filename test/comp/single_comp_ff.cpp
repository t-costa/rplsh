// pipe(source_vec_stage,c,drain_vec_stage)

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
// #include <aux/ff_comp.hpp>

// business code headers
#include </home/tommaso/forked/rplsh/test/definition.hpp>

using namespace ff;


int nw = 1;
class source_vec_stage_stage : public ff_node {
protected:
	// std::unique_ptr<source_vec_stage> src;
	source_vec_stage src;

public:
	// source_vec_stage_stage() : src(new source_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src.has_next() )
			ff_send_out((void*) src.next() );
		return (EOS);
	}
};

class drain_vec_stage_stage : public ff_node {
protected:
	// std::unique_ptr<drain_vec_stage> drn;
	drain_vec_stage drn;

public:
	// drain_vec_stage_stage() : drn(new drain_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn.process((std::vector<utils::elem_type>*) t);
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

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2";
	threadMapper::instance()->setMappingList(worker_mapping);

	source_vec_stage_stage _source_vec_stage;
	seq_vec_vec_stage_stage _seq_vec_vec_stage;
	drain_vec_stage_stage _drain_vec_stage;
	// ff_pipeline pipe;
	// pipe.add_stage(&_source_vec_stage);
	// pipe.add_stage(&_seq_vec_vec_stage);
	// pipe.add_stage(&_drain_vec_stage);

	ff_comb comp1(_source_vec_stage, _seq_vec_vec_stage);
	ff_comb comp2(comp1, _drain_vec_stage);

	comp2.run_and_wait_end();
	std::cout << "nw = " << nw << ". ";
	std::cout << "Spent: " << comp2.ffTime() << " msecs" << std::endl;

	// pipe.run_and_wait_end();
	// std::cout << "nw = " << nw << ". ";
	// std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	// #ifdef TRACE_FASTFLOW
	// std::cout << "Stats: " << std::endl;
	// pipe.ffStats(std::cout);
	// #endif


	// utils::build_json(0, par_time, par_degree, "pipe(source_vec_stage,c,drain_vec_stage)");
	return 0;

}
