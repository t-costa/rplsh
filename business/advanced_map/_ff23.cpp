// pipe(dsrc,m,ddrn)

#include <iostream>
#include <vector>

// specify include directory for fastflow
#include <ff/ff.hpp>
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
		return (GO_ON);
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
	map0_stage() : ff_Map(1) {
		pfr.disableScheduler(0);
	}

	vec* svc(vec_pair *t) {
		vec_pair& _task = *t;
		vec* out = new vec();
		out->resize(_task.size());
		ff_Map<vec_pair,vec>::parallel_for(0, _task.size(),[this, &_task, &out](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			auto res1 = wrapper1.op(res0);
			auto res2 = wrapper2.op(res1);
			auto res3 = wrapper3.op(res2);
			(*out)[i] = wrapper4.op(res3);
		},1);
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
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
