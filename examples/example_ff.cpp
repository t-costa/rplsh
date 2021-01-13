// pipe(comp(s,map(comp(a,b)) with [ nw: 9]),farm(reduce(comp(b,c)) with [ nw: 58]) with [ nw: 1],comp(i,divide_and_conquer(f) with [nw 1]),comp(farm(g) with [ nw: 1],map(seq() with [ ts: 4.000000]) with [ nw: 8],farm(divide_and_conquer(h) with [nw 5]) with [ nw: 1]),reduce(l) with [ nw: 38],d)

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

class map0_stage : public ff_Map<matrix_couple,matrix> {
protected:
	map_matrix_mul_stage wrapper0;
	map_matrix wrapper1;
public:
	map0_stage() : ff_Map(9) {
	}

	matrix* svc(matrix_couple *t) {
		matrix_couple& _task = *t;
		matrix* out = new matrix();
		out->resize(_task.size());
		pfr.parallel_for(0, _task.size(), 1, 300, [this, &_task, &out](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			(*out)[i] = wrapper1.op(res0);
		},9);

		delete t;

		return out;
	}
};

class map1_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_vec_vec_stage wrapper0;
public:
	map1_stage() : ff_Map(8) {
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		pfr.parallel_for_static(0, _task.size(), 1, 0, [this, &_task, &out](const long i) {
			(*out)[i] = wrapper0.op(_task[i]);
		},8);

		return out;
	}
};

class reduce0_stage : public ff_Map<matrix,std::vector<utils::elem_type>, std::vector<utils::elem_type>> {
protected:
	map_matrix wrapper0;
	reduce_matrix_vec_stage wrapper1;
public:
	reduce0_stage() : ff_Map(58) {
	}

	std::vector<utils::elem_type>* svc(matrix* t) {
		matrix& _task = *t;
		std::vector<utils::elem_type>* out  = new std::vector<utils::elem_type>(wrapper1.identity);
		auto reduceF = [this](std::vector<utils::elem_type>& sum, std::vector<utils::elem_type> elem) {sum = wrapper1.op(sum, elem);};
		matrix* mapout = new matrix();
		mapout->resize(_task.size());
		pfr.parallel_for_static(0, _task.size(), 1, 0, [this, &_task, &mapout](const long i) {
			(*mapout)[i] = wrapper0.op(_task[i]);
		},58);
		auto bodyF = [this,&mapout](const long i, std::vector<utils::elem_type>& sum) {sum = wrapper1.op(sum, (*mapout)[i]);};
		pfr.parallel_reduce_static(*out, wrapper1.identity, 0, mapout->size(), 1, 0, bodyF, reduceF, 58);

		delete mapout;

		delete t;

		return out;
	}
};

class reduce1_stage : public ff_Map<std::vector<utils::elem_type>,utils::elem_type, utils::elem_type> {
protected:
	reduce_vec_double_stage wrapper0;
public:
	reduce1_stage() : ff_Map(38) {
	}

	utils::elem_type* svc(std::vector<utils::elem_type>* t) {
		std::vector<utils::elem_type>& _task = *t;
		utils::elem_type* out  = new utils::elem_type(wrapper0.identity);
		auto reduceF = [this](utils::elem_type& sum, utils::elem_type elem) {sum = wrapper0.op(sum, elem);};
		auto bodyF = [this,&_task](const long i, utils::elem_type& sum) {sum = wrapper0.op(sum, _task[i]);};
		pfr.parallel_reduce_static(*out, wrapper0.identity, 0, _task.size(), 1, 0, bodyF, reduceF, 38);

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,0,1,2,3,4,5,6,7,8,9,10,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,74,77,78,79,77,78,79,80,81,82,83,84,85,86,77,78,79,80,81,82,83,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_matrixpair_stage_stage _source_matrixpair_stage;
	map0_stage _map0_;
	ff_pipeline pipe1;
	pipe1.add_stage(&_source_matrixpair_stage);
	pipe1.add_stage(&_map0_);


	// vector of workers of farm
	std::vector<ff_node*> workers;

	// farm's worker 1
	reduce0_stage _red0_;
	workers.push_back(&_red0_);

	// add workers to farm
	ff_farm farm;
	farm.add_workers(workers);
	farm.add_collector(NULL);

	seq_vec_vec_stage_stage _seq_vec_vec_stage;
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
	1);
	ff_pipeline pipe2;
	pipe2.add_stage(&_seq_vec_vec_stage);
	pipe2.add_stage(&_dc0_);


	// vector of workers of farm1
	std::vector<ff_node*> workers1;

	// farm1's worker 1
	seq_vec_vec_stage_stage _seq_vec_vec_stage1;
	workers1.push_back(&_seq_vec_vec_stage1);

	// add workers1 to farm1
	ff_farm farm1;
	farm1.add_workers(workers1);
	farm1.add_collector(NULL);

	map1_stage _map1_;

	// vector of workers of farm2
	std::vector<ff_node*> workers2;

	// farm2's worker 1
	map_vec_vec_stage dc_stage1;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_1(
		//divide function
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			auto half_size = in.size() / 2;
			in_vec.emplace_back(in.begin(), in.begin() + half_size);
			in_vec.emplace_back(in.begin() + half_size, in.end());
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
			out = dc_stage1.compute(in_arg);
		},
		//condition function
		[&](const std::vector<utils::elem_type>& in) {
			return in.size() <= 1;
		},
	5);
	workers2.push_back(&_dc0_1);

	// add workers2 to farm2
	ff_farm farm2;
	farm2.add_workers(workers2);
	farm2.add_collector(NULL);

	ff_pipeline pipe3;
	pipe3.add_stage(&farm1);
	pipe3.add_stage(&_map1_);
	pipe3.add_stage(&farm2);

	reduce1_stage _red1_;
	drain_stage_stage _drain_stage;
	ff_pipeline pipe;
	pipe.add_stage(&pipe1);
	pipe.add_stage(&farm);
	pipe.add_stage(&pipe2);
	pipe.add_stage(&pipe3);
	pipe.add_stage(&_red1_);
	pipe.add_stage(&_drain_stage);


	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}
