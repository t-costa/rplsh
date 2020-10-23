// seq() with [ ts: 1.000000]

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
		std::vector<double> _in  = *((std::vector<double>*) t);
		std::vector<double>* _out  = new std::vector<double>();
		*_out = wrapper.compute(_in);
		delete ((std::vector<double>*) t);
		return (void*) _out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0";
	threadMapper::instance()->setMappingList(worker_mapping);
	seq_vec_vec_stage_stage _seq_vec_vec_stage;
	
	_seq_vec_vec_stage.run_and_wait_end();
	std::cout << "Spent: " << _seq_vec_vec_stage.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	_seq_vec_vec_stage.ffStats(std::cout);
	#endif
	return 0;
	
}
