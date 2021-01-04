// pipe(source_matrixpair_stage,m4,drain_matrix_stage)

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
#include </home/tommaso/forked/rplsh/test/definition.hpp>


class source_matrixpair_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_matrixpair_stage> src;

public:
	source_matrixpair_stage_stage() : src(new source_matrixpair_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_matrixpair_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_matrix_stage_stage : public ff_node {
protected:
	std::unique_ptr<drain_matrix_stage> drn;

public:
	drain_matrix_stage_stage() : drn(new drain_matrix_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_matrix_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((matrix*) t);
		return (GO_ON);
	}
};

class map0_stage : public ff_Map<matrix_couple,matrix> {
protected:
	map_matrix_mul_stage wrapper0;
public:
	map0_stage() : ff_Map(4) {
	}

	matrix* svc(matrix_couple *t) {
		matrix_couple& _task = *t;
		matrix* out = new matrix();
		out->resize(_task.size());
		pfr.parallel_for_static(0, _task.size(), 1, 0, [this, &_task, &out](const long i) {
			(*out)[i] = wrapper0.op(_task[i]);
		},4);

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2,3,4";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_matrixpair_stage_stage _source_matrixpair_stage;
	map0_stage _map0_;
	drain_matrix_stage_stage _drain_matrix_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_matrixpair_stage);
	pipe.add_stage(&_map0_);
	pipe.add_stage(&_drain_matrix_stage);


	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}
