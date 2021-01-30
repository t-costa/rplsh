// pipe(comp(source_vec,c,seq_vec_vec),dc,rcr,drain_elem)

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
#include </home/costanzo/rplsh/examples/example_code.hpp>


class source_vec_stage : public ff_node {
protected:
	std::unique_ptr<source_vec> src; 

public:
	source_vec_stage() : src(new source_vec()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_vec -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_elem_stage : public ff_node {
protected:
	std::unique_ptr<drain_elem> drn; 

public:
	drain_elem_stage() : drn(new drain_elem()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_elem -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((utils::elem_type*) t);
		return (GO_ON);
	}
};

class seq_vec_vec_stage : public ff_node {
protected:
	seq_vec_vec wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- seq_vec_vec -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
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

class map0_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_vec_vec wrapper0;
	map_vec_vec wrapper1;
public:
	map0_stage() : ff_Map(4) {
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			(*out)[i] = wrapper1.op(res0);
		},4);

		return out;
	}
};

class reduce0_stage : public ff_Map<std::vector<utils::elem_type>,utils::elem_type, utils::elem_type> {
protected:
	map_minus wrapper0;
	reduce_vec_double wrapper1;
public:
	reduce0_stage() : ff_Map(5) {
	}

	utils::elem_type* svc(std::vector<utils::elem_type>* t) {
		std::vector<utils::elem_type>& _task = *t;
		utils::elem_type* out  = new utils::elem_type(wrapper1.identity);
		auto reduceF = [this](utils::elem_type& sum, utils::elem_type elem) {sum = wrapper1.op(sum, elem);};
		std::vector<utils::elem_type>* mapout = new std::vector<utils::elem_type>();
		mapout->resize(_task.size());
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &mapout, step](const long i) {
			(*mapout)[i] = wrapper0.op(_task[i]);
		},5);
		auto bodyF = [this,&mapout](const long i, utils::elem_type& sum) {sum = wrapper1.op(sum, (*mapout)[i]);};
		pfr.parallel_reduce_static(*out, wrapper1.identity, 0, mapout->size(), 1, 0, bodyF, reduceF, 5);

		delete mapout;

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,0,1,2,3,4,5,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_vec_stage _source_vec;
	map0_stage _map0_;
	
	// vector of workers of farm
	std::vector<ff_node*> workers;
	
	// farm's worker 1
	seq_vec_vec_stage _seq_vec_vec;
	workers.push_back(&_seq_vec_vec);
	
	// farm's worker 2
	seq_vec_vec_stage _seq_vec_vec1;
	workers.push_back(&_seq_vec_vec1);
	
	// farm's worker 3
	seq_vec_vec_stage _seq_vec_vec2;
	workers.push_back(&_seq_vec_vec2);
	
	// farm's worker 4
	seq_vec_vec_stage _seq_vec_vec3;
	workers.push_back(&_seq_vec_vec3);
	
	// farm's worker 5
	seq_vec_vec_stage _seq_vec_vec4;
	workers.push_back(&_seq_vec_vec4);
	
	// add workers to farm
	ff_farm farm;
	farm.add_workers(workers);
	farm.add_collector(nullptr);
	
	ff_pipeline pipe1;
	pipe1.add_stage(&_source_vec);
	pipe1.add_stage(&_map0_);
	pipe1.add_stage(&farm);
	
	dc_dummy dc_stage;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage.combine(out_vec, out);
		},
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			dc_stage.seq(in, out);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return dc_stage.cond(in);
		},
	47);
	reduce0_stage _red0_;
	drain_elem_stage _drain_elem;
	ff_pipeline pipe;
	pipe.add_stage(&pipe1);
	pipe.add_stage(&_dc0_);
	pipe.add_stage(&_red0_);
	pipe.add_stage(&_drain_elem);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
