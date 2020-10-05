// pipe(dsrc,m,ddrn)

#include <iostream>
#include <vector>

// specify include directory for fastflow
#include <ff/farm.hpp>
#include <ff/map.hpp>
#include <ff/pipeline.hpp>
#include <ff/parallel_for.hpp>

// specify include directory for RPL-Shell
#include <aux/types.hpp>
#include <aux/wrappers.hpp>
#include <aux/ff_comp.hpp>

// business code headers
#include </home/tommaso/forked/rplsh/business/advanced_map/business.hpp>


class dsrc_stage : public ff_node {
protected:
	std::unique_ptr<dsrc> src; 

public:
	dsrc_stage() : src(new dsrc()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- dsrc -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class ddrn_stage : public ff_node {
protected:
	std::unique_ptr<ddrn> drn; 

public:
	ddrn_stage() : drn(new ddrn()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- ddrn -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((vec*) t);
		delete (vec*) t;
		return (GO_ON);
	}
};

class map_first_stage : public ff_node {
protected:
	map_first wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- map_first -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		vec_pair _in  = *((vec_pair*) t);
		vec* _out  = new vec();
		*_out = wrapper.compute(_in);
		delete ((vec_pair*) t);
		return (void*) _out;
	}
};

class map_second_stage : public ff_node {
protected:
	map_second wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- map_second -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		vec _in  = *((vec*) t);
		vec* _out  = new vec();
		*_out = wrapper.compute(_in);
		delete ((vec*) t);
		return (void*) _out;
	}
};

class map_third_stage : public ff_node {
protected:
	map_third wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- map_third -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		vec _in  = *((vec*) t);
		vec* _out  = new vec();
		*_out = wrapper.compute(_in);
		delete ((vec*) t);
		return (void*) _out;
	}
};

class map_fourth_stage : public ff_node {
protected:
	map_fourth wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- map_fourth -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		vec _in  = *((vec*) t);
		vec* _out  = new vec();
		*_out = wrapper.compute(_in);
		delete ((vec*) t);
		return (void*) _out;
	}
};

class map_fifth_stage : public ff_node {
protected:
	map_fifth wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- map_fifth -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		vec _in  = *((vec*) t);
		vec* _out  = new vec();
		*_out = wrapper.compute(_in);
		delete ((vec*) t);
		return (void*) _out;
	}
};

class map0_stage : public ff_Map<vec_pair,vec> {
protected:
	map_first wrapper0;
	map_second wrapper1;
	map_third wrapper2;
	map_fourth wrapper3;
	map_fifth wrapper4;
public:
	map0_stage() : ff_Map() {
		pfr.disableScheduler(0);
	}

	vec* svc(vec_pair *t) {
		vec_pair& _task = *t;
		vec* out = new vec();
		out->resize(_task.size());

		// ff_Map<vec_pair, vec>::parallel_for(0, _task.size(), [this, &_task, &out](const long i) {
		// 	auto res0 = wrapper0.op(_task[i]);
		// 	auto res1 = wrapper1.op(res0);
		// 	auto res2 = wrapper2.op(res1);
		// 	auto res3 = wrapper3.op(res2);
		// 	(*out)[i] = wrapper4.op(res3);
		// }, 4);

		//ParallelFor pf;

		
		//first hook
		vec tmp_out_first(_task.size());
		ff_Map<vec_pair,vec>::parallel_for(0, _task.size(),[this, &_task, &out, &tmp_out_first](const long i) {
			tmp_out_first[i] = wrapper0.op(_task[i]);
		}, 4);

		//second hook
		vec tmp_out_second(tmp_out_first.size());
		ff_Map<vec_pair, vec>::parallel_for(0, tmp_out_first.size(),[this, &tmp_out_first, &tmp_out_second](const long i) {
			tmp_out_second[i] = wrapper1.op(tmp_out_first[i]);
		}, 4);

		//third hook
		vec tmp_out_third(tmp_out_second.size());
		ff_Map<vec_pair, vec>::parallel_for(0, tmp_out_second.size(),[this, &tmp_out_second, &tmp_out_third](const long i) {
			tmp_out_third[i] = wrapper2.op(tmp_out_second[i]);
		}, 4);

		//fourth hook
		vec tmp_out_fourth(tmp_out_third.size());
		ff_Map<vec_pair, vec>::parallel_for(0, tmp_out_third.size(),[this, &tmp_out_third, &tmp_out_fourth](const long i) {
			tmp_out_fourth[i] = wrapper3.op(tmp_out_third[i]);
		}, 4);

		//fifth hook
		ff_Map<vec_pair, vec>::parallel_for(0, tmp_out_fourth.size(),[this, &tmp_out_fourth, &out](const long i) {
			(*out)[i] = wrapper4.op(tmp_out_fourth[i]);
		}, 4);

		delete t;
		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4";
	threadMapper::instance()->setMappingList(worker_mapping);
	dsrc_stage _dsrc;
	map0_stage _map0_;
	ddrn_stage _ddrn;
	ff_pipeline pipe;
	pipe.add_stage(&_dsrc);
	pipe.add_stage(&_map0_);
	pipe.add_stage(&_ddrn);
	

	dsrc source;
    ddrn drain;
    map_first m1;
    map_second m2;
    map_third m3;
    map_fourth m4;
    map_fifth m5;

	auto start = aux::high_resolution_clock::now();
    while (source.has_next()) {
        auto v = source.next();
        auto v1 = m1.compute(*v);
        auto v2 = m2.compute(v1);
        auto v3 = m3.compute(v2);
        auto v4 = m4.compute(v3);
        auto v5 = m5.compute(v4);
        drain.process(&v5);
    }
	auto end = aux::high_resolution_clock::now();
	auto t_seq = aux::time_elapsed<aux::milliseconds>(end, start);
	std::cout << "Sequential time: " << t_seq << " msec" << std::endl;

	sequential = false;
	seed = 1;

#ifdef DEBUG
result_store::check_store<vec>();
#endif
	pipe.run_and_wait_end();
	std::cout << "FastFlow time: " << pipe.ffTime() << " msecs" << std::endl;
	
	std::cout << "Speedup: " << t_seq / pipe.ffTime() << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats:" << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
