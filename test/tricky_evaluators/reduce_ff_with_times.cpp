// pipe(source_vec_stage,r,drain_stage)

#include <iostream>
#include <vector>
#include <mutex>
#include <chrono>

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
#include </home/tommaso/forked/rplsh/test/definition.hpp>


std::mutex mtx;
std::vector<std::chrono::high_resolution_clock::time_point> times;

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

class reduce0_stage : public ff_Map<std::vector<utils::elem_type>,utils::elem_type> {
protected:
	reduce_stage wrapper0;
public:
	reduce0_stage() : ff_Map(4) {
	}

	utils::elem_type* svc(std::vector<utils::elem_type>* t) {

		mtx.lock();
		times.emplace_back(std::chrono::high_resolution_clock::now());
		mtx.unlock();

		std::vector<utils::elem_type>& _task = *t;
		utils::elem_type* out  = new utils::elem_type(wrapper0.identity);
		auto reduceF = [this](utils::elem_type& sum, utils::elem_type elem) {sum = wrapper0.op(sum, elem);};
		auto bodyF = [this,&_task](const long i, utils::elem_type& sum) {sum = wrapper0.op(sum, _task[i]);};
		pfr.parallel_reduce_static(*out, wrapper0.identity, 0, _task.size(), 1, 0, bodyF, reduceF, 4);

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	// const char worker_mapping[] = "0,1,2";
	// threadMapper::instance()->setMappingList(worker_mapping);
	source_vec_stage_stage _source_vec_stage;
	reduce0_stage _red0_;
	drain_stage_stage _drain_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_vec_stage);
	pipe.add_stage(&_red0_);
	pipe.add_stage(&_drain_stage);

	times.reserve(parameters::dimension);

	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif

	//prendiamo la media dei tempi di arrivo
	// auto res = times[0];
	// for(auto i=1; i<times.size(); ++i) {
	// 	res += times[i];
	// }
	for (size_t i=0; i<times.size()-5; i+=5) {
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(times[i+1] - times[i]).count() << " microsec \t";
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(times[i+2] - times[i+1]).count() << " microsec \t";
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(times[i+3] - times[i+2]).count() << " microsec \t";
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(times[i+4] - times[i+3]).count() << " microsec \t";
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(times[i+5] - times[i+4]).count() << " microsec\n";
	}

	return 0;

}
