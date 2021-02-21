// pipe(s,farm(mr) with [ nw: 1],d)

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
// business code headers
#include </home/tommaso/forked/rplsh/test/definition.hpp>

using namespace ff;

class source_matrix_vectors_stage : public ff_node {
protected:
	source_matrix_vectors src;

public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_matrix_vectors -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src.has_next() )
			ff_send_out((void*) src.next() );
		return (EOS);
	}
};

class drain_matrixelem_stage : public ff_node {
protected:
	drain_matrixelem drn;

public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_matrixelem -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn.process((utils::elem_idx_idx*) t);
		return (GO_ON);
	}
};

class reduce0_stage : public ff_Map<std::vector<utils::elem_elem_idx_idx>,utils::elem_idx_idx, utils::elem_idx_idx> {
protected:
	map_vec_prod wrapper0;
	red_sum wrapper1;
public:
	reduce0_stage() : ff_Map(1) {
	}

	utils::elem_idx_idx* svc(std::vector<utils::elem_elem_idx_idx>* t) {
		std::vector<utils::elem_elem_idx_idx>& _task = *t;
		utils::elem_idx_idx* out  = new utils::elem_idx_idx(wrapper1.identity);
		auto bodyF = [this,&_task](const long i, utils::elem_idx_idx& sum) {
			auto res0 = wrapper0.op(_task[i]);
			sum = wrapper1.op(sum, res0);
		};
		auto reduceF = [this](utils::elem_idx_idx& sum, utils::elem_idx_idx elem) {sum = wrapper1.op(sum, elem);};
		pfr.parallel_reduce_static(*out, wrapper1.identity, 0, _task.size(), 1, 0, bodyF, reduceF, 1);

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5,6";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_matrix_vectors_stage _source_matrix_vectors;
	
	// vector of workers of farm
	std::vector<ff_node*> workers;
	
	// farm's worker 1
	reduce0_stage _red0_;
	workers.push_back(&_red0_);
	
	// add workers to farm
	ff_farm farm;
	farm.add_workers(workers);
	farm.add_collector(nullptr);
	
	drain_matrixelem_stage _drain_matrixelem;
	ff_pipeline pipe;
	pipe.add_stage(&_source_matrix_vectors);
	pipe.add_stage(&farm);
	pipe.add_stage(&_drain_matrixelem);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
