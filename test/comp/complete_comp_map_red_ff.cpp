// comp(s,m,r,d)

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

class map_vec_prod_stage : public ff_node {
protected:
	map_vec_prod wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- map_vec_prod -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		std::vector<utils::elem_elem_idx_idx> _in  = *((std::vector<utils::elem_elem_idx_idx>*) t);
		std::vector<utils::elem_idx_idx>* _out  = new std::vector<utils::elem_idx_idx>();
		*_out = wrapper.compute(_in);
		delete ((std::vector<utils::elem_elem_idx_idx>*) t);
		return (void*) _out;
	}
};

class red_sum_stage : public ff_node {
protected:
	red_sum wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- red_sum -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		std::vector<utils::elem_idx_idx> _in  = *((std::vector<utils::elem_idx_idx>*) t);
		utils::elem_idx_idx* _out  = new utils::elem_idx_idx();
		*_out = wrapper.compute(_in);
		delete ((std::vector<utils::elem_idx_idx>*) t);
		return (void*) _out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,0,0,0";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_matrix_vectors_stage _source_matrix_vectors;
	map_vec_prod_stage _map_vec_prod;
	red_sum_stage _red_sum;
	drain_matrixelem_stage _drain_matrixelem;
	ff_comb comp(_source_matrix_vectors, _map_vec_prod);
	ff_comb comp1(comp, _red_sum);
	ff_comb comp2(comp1, _drain_matrixelem);
	
	comp2.run_and_wait_end();
	std::cout << "Spent: " << comp2.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	comp2.ffStats(std::cout);
	#endif
	return 0;
	
}
