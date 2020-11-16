// pipe(source_vec_stage,reduce(comp(map(map_vec_vec_stage) with [ nw: 1],reduce_vec_double_stage)) with [ nw: 1],drain_stage)

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
#include </home/tommaso/forked/rplsh/test/complete/definition.hpp>


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

class reduce_vec_double_stage_stage : public ff_node {
protected:
	reduce_vec_double_stage wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- reduce_vec_double_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		std::vector<utils::elem_type> _in  = *((std::vector<utils::elem_type>*) t);
		utils::elem_type* _out  = new utils::elem_type();
		*_out = wrapper.compute(_in);
		delete ((std::vector<utils::elem_type>*) t);
		return (void*) _out;
	}
};

class reduce0_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
protected:
	map_vec_vec_stage wrapper0;
public:
	reduce0_stage() : ff_Map(1) {
		pfr.disableScheduler(1);
	}

	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type>* t) {
		std::vector<utils::elem_type>& _task = *t;
		std::vector<utils::elem_type>* out  = new std::vector<utils::elem_type>(wrapper0.identity);
		auto reduceF = [this](std::vector<utils::elem_type>& sum, std::vector<utils::elem_type> elem) {sum = wrapper0.op(sum, elem);};
		auto bodyF = [this,&_task](const long i, std::vector<utils::elem_type>& sum) {sum = wrapper0.op(sum, _task[i]);};
		ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>,std::vector<utils::elem_type>>::parallel_reduce(*out, wrapper0.identity,0,_task.size(),bodyF,reduceF,1);
		
return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_vec_stage_stage _source_vec_stage;
	reduce0_stage _red0_;
	drain_stage_stage _drain_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_vec_stage);
	pipe.add_stage(&_red0_);
	pipe.add_stage(&_drain_stage);
	
	
	parameters::sequential = false;
	utils::write("pipe(source_vec_stage,reduce(comp(map(map_vec_vec_stage) with [ nw: 1],reduce_vec_double_stage)) with [ nw: 1],drain_stage)", "./res_ff.txt");
	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	utils::write("\n---------------------\n", "./res_ff.txt");
	return 0;
	
}
