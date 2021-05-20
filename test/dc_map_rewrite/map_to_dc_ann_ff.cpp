// pipe(source_ordered_vec_stage,m,mtd,drain_ordered_vec_stage)

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

class source_ordered_vec_stage_stage : public ff_node {
protected:
	source_ordered_vec_stage src;

public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_ordered_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src.has_next() )
			ff_send_out((void*) src.next() );
		return (EOS);
	}
};

class drain_ordered_vec_stage_stage : public ff_node {
protected:
	drain_ordered_vec_stage drn;

public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_ordered_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn.process((std::vector<utils::elem_type>*) t);
		return (GO_ON);
	}
};

class map0_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_vec_vec_stage wrapper0;
public:
	map0_stage() : ff_Map(1) {
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			(*out)[i] = wrapper0.op(_task[i]);
		},1);

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_ordered_vec_stage_stage _source_ordered_vec_stage;
	map0_stage _map0_;
	map_vec_vec_stage dc_stage;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_(
		//divide function
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			size_t schedule = 100;
			in_vec.resize(schedule);
			//tie scheduling
			auto new_size = in.size() / schedule;
			size_t j = 0;
			for (size_t i=0; i<in.size(); ++i) {
				if (i >= (j+1)*new_size && j<schedule-1)
					j++;
				in_vec[j].push_back(in[i]);
			}
		},
		//combine function
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			size_t schedule = 100;
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
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			auto in_arg = in;
			out = dc_stage.compute(in_arg);
		},
		//condition function
		[&](const std::vector<utils::elem_type>& in) {
			return in.size() <= 10;
		},
	1);
	drain_ordered_vec_stage_stage _drain_ordered_vec_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_ordered_vec_stage);
	pipe.add_stage(&_map0_);
	pipe.add_stage(&_dc0_);
	pipe.add_stage(&_drain_ordered_vec_stage);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
