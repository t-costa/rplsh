// pipe(map(reduce(comp(s1,s2)) with [ nw: 2]) with [ nw: 5],farm(s3) with [ nw: 1])

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
#include <>


class _stage : public ff_node {
protected:
	 wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init --  -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		 _in  = *((*) t);
		* _out  = new ();
		*_out = wrapper.compute(_in);
		delete ((*) t);
		return (void*) _out;
	}
};

class _stage : public ff_node {
protected:
	 wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init --  -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		 _in  = *((*) t);
		* _out  = new ();
		*_out = wrapper.compute(_in);
		delete ((*) t);
		return (void*) _out;
	}
};

class _stage : public ff_node {
protected:
	 wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init --  -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		 _in  = *((*) t);
		* _out  = new ();
		*_out = wrapper.compute(_in);
		delete ((*) t);
		return (void*) _out;
	}
};

class map0_stage : public ff_Map<,> {
protected:
	 wrapper0;
	 wrapper1;
public:
	map0_stage() : ff_Map(5) {
		pfr.disableScheduler(0);
	}

	* svc( *t) {
		& _task = *t;
		* out = &_task;
		ff_Map<,>::parallel_for(0, _task.size(),[this, &_task](const long i) {
			auto res0 = wrapper0.op(_task[i]);
			wrapper1.op(res0);
		},10);
		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14";
	threadMapper::instance()->setMappingList(worker_mapping);
	map0_stage _map0_;
	
	// vector of workers of farm
	std::vector<ff_node*> workers;
	
	// farm's worker 1
	_stage _;
	workers.push_back(&_);
	
	// add workers to farm
	ff_farm<> farm;
	farm.add_workers(workers);
	farm.add_collector(NULL);
	
	ff_pipeline pipe;
	pipe.add_stage(&_map0_);
	pipe.add_stage(&farm);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent:" << pipe.ffTime() << "msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats:" << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
