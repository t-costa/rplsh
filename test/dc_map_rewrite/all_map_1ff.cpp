// pipe(source_ordered_vec_stage,m,m1,m2,m3,m4,cc,drain_ordered_vec_stage)

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


class source_ordered_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_ordered_vec_stage> src;

public:
	source_ordered_vec_stage_stage() : src(new source_ordered_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_ordered_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_ordered_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<drain_ordered_vec_stage> drn;

public:
	drain_ordered_vec_stage_stage() : drn(new drain_ordered_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_ordered_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
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
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			(*out)[i] = wrapper0.op(_task[i]);
		},1);

		return out;
	}
};

class map1_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_vec_vec_stage wrapper0;
public:
	map1_stage() : ff_Map(1) {
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		size_t step = 4;
		pfr.parallel_for(0, _task.size(), step, 7, [this, &_task, &out, step](const long i) {
			std::vector<utils::elem_type> tmp;
			for (size_t j=0; j<step && (i+j)<_task.size(); ++j) {
				tmp.push_back(_task[i+j]);
			}
			auto partial = wrapper0.compute(tmp);
			for (size_t j=0; j<step &&(i+j)<_task.size(); ++j) {
				(*out)[i+j] = partial[j];
			}
		},1);

		return out;
	}
};

class map2_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_vec_vec_stage wrapper0;
	map_vec_vec_stage wrapper1;
	map_vec_vec_stage wrapper2;
public:
	map2_stage() : ff_Map(1) {
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			auto res1 = wrapper1.op(res0);
			(*out)[i] = wrapper2.op(res1);
		},1);

		return out;
	}
};

class map3_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_vec_vec_stage wrapper0;
	map_vec_vec_stage wrapper1;
	map_vec_vec_stage wrapper2;
public:
	map3_stage() : ff_Map(1) {
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		size_t step = 3;
		pfr.parallel_for(0, _task.size(), step, 7, [this, &_task, &out, step](const long i) {
			std::vector<utils::elem_type> tmp;
			for (size_t j=0; j<step && (i+j)<_task.size(); ++j) {
				tmp.push_back(_task[i+j]);
			}
			auto res0 = wrapper0.compute(tmp);
			auto res1 = wrapper1.compute(res0);
			auto partial = wrapper2.compute(res1);
			for (size_t j=0; j<step &&(i+j)<_task.size(); ++j) {
				(*out)[i+j] = partial[j];
			}
		},1);

		return out;
	}
};

class map4_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	dc_dummy wrapper0;
public:
	map4_stage() : ff_Map(1) {
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			std::vector<utils::elem_type> tmp;
			for (size_t j=0; j<step && (i+j)<_task.size(); ++j) {
				tmp.push_back(_task[i+j]);
			}
			auto partial = wrapper0.compute(tmp);
			for (size_t j=0; j<step &&(i+j)<_task.size(); ++j) {
				(*out)[i+j] = partial[j];
			}
		},1);

		return out;
	}
};

class map5_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_vec_vec_stage wrapper0;
	dc_dummy wrapper1;
public:
	map5_stage() : ff_Map(1) {
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out = &_task;
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			// auto res0 = wrapper0.op(_task[i]);
			// (*out)[i] = wrapper1.op(res0);
			std::vector<utils::elem_type> tmp;
			for (size_t j=0; j<step && (i+j)<_task.size(); ++j) {
				tmp.push_back(_task[i+j]);
			}
			auto partial = wrapper0.compute(tmp);
			auto res1 = wrapper1.compute(partial);
			for (size_t j=0; j<step &&(i+j)<_task.size(); ++j) {
				(*out)[i+j] = res1[j];
			}
		},1);

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_ordered_vec_stage_stage _source_ordered_vec_stage;
	map0_stage _map0_;
	map1_stage _map1_;
	map2_stage _map2_;
	map3_stage _map3_;
	map4_stage _map4_;
	map5_stage _map5_;
	drain_ordered_vec_stage_stage _drain_ordered_vec_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_ordered_vec_stage);
	pipe.add_stage(&_map0_);
	pipe.add_stage(&_map1_);
	pipe.add_stage(&_map2_);
	pipe.add_stage(&_map3_);
	pipe.add_stage(&_map4_);
	pipe.add_stage(&_map5_);
	pipe.add_stage(&_drain_ordered_vec_stage);


	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}
