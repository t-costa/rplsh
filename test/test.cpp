// pipe(source_vec_stage,n,t,drain_vec_stage)

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
#include </home/tommaso/forked/rplsh/test/definition.hpp>


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

class drain_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<drain_vec_stage> drn; 

public:
	drain_vec_stage_stage() : drn(new drain_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((std::vector<utils::elem_type>*) t);
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
		pfr.parallel_for_static(0, _task.size(), 1, 0, [this, &_task, &out](const long i) {
			(*out)[i] = wrapper0.op(_task[i]);
		},1);

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_vec_stage_stage _source_vec_stage;
	map0_stage _map0_;
	map_vec_vec_stage dc_stage;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_(
		//divide function
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			auto half_size = in.size() / 2;
			std::vector<utils::elem_type> a, b;
			std::copy(in.begin(), in.begin() + half_size, std::back_inserter(a));
			std::copy(in.begin() + half_size, in.end(), std::back_inserter(b));
			in_vec.push_back(a);
			in_vec.push_back(b);
		},
		//combine function
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			out.resize(out_vec[0].size() + out_vec[1].size());
			size_t i = 0;
			for(auto& a : out_vec[0]) {
				out[i] = a;
				i++;
			}
			for(auto& b : out_vec[1]) {
				out[i] = b;
				i++;
			}
		},
		//sequential case function
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			auto in_arg = in;
			out = dc_stage.compute(in_arg);
		},
		//condition function
		[&](const std::vector<utils::elem_type>& in) {
			return in.size() <= 1;
		},
	1);
	drain_vec_stage_stage _drain_vec_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_vec_stage);
	pipe.add_stage(&_map0_);
	pipe.add_stage(&_dc0_);
	pipe.add_stage(&_drain_vec_stage);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
