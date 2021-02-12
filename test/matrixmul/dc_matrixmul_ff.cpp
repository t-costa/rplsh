// pipe(source_matrixpair_stage,m,drain_matrix_stage)

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
		while( src->has_next() ) {
			ff_send_out((void*) src->next() );
		}
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

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_matrixpair_stage_stage _source_matrixpair_stage;
	map_matrix_mul_stage dc_stage;
	ff_DC<matrix_couple, matrix> _dc0_(
		//divide function
		[&](const matrix_couple& in, std::vector<matrix_couple>& in_vec) {
			size_t schedule = 2;
			in_vec.resize(schedule);
			//tie scheduling
			auto new_size = in.size() / schedule;
			size_t j = 0;
			for (size_t i=0; i<in.size(); ++i) {
				if (i >= (j+1)*new_size && j<schedule-1)
					j++;
				// auto pass = in[i];
				in_vec[j].push_back(in[i]);
			}
		},
		//combine function
		[&](std::vector<matrix>& out_vec, matrix& out) {
			size_t schedule = 2;
			size_t final_size = 0;
			//compute size of out vector
			for (size_t k=0; k<schedule; ++k) {
				final_size += out_vec[k].size();
			}
			out.resize(final_size);
			//combine results
			size_t i = 0, j = 0;
			// tie merge
			for (j=0; j<schedule; ++j) {
				for(auto& a : out_vec[j]) {
					out[i] = a;
					i++;
				}
			}
		},
		//sequential case function
		[&](const matrix_couple& in, matrix& out) {
			auto in_arg = in;
			out = dc_stage.compute(in_arg);
		},
		//condition function
		[&](const matrix_couple& in) {
			return in.size() <= 1;
		},
	3);
	drain_matrix_stage_stage _drain_matrix_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_matrixpair_stage);
	pipe.add_stage(&_dc0_);
	pipe.add_stage(&_drain_matrix_stage);


	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	std::cout << "dc" << std::endl;
	return 0;

}
