// pipe(generator,farm(sorter) with [ nw: 6],map(multiplier) with [ nw: 3],reduce(sum) with [ nw: 2])

#include <iostream>
#include <vector>

// specify include directory for fastflow
#include <ff/farm.hpp>
#include <ff/map.hpp>
#include <ff/pipeline.hpp>
#include <ff/parallel_for.hpp>

// specify include directory for RPL-Shell
#include <types.hpp>
#include <wrappers.hpp>
#include <ff_comp.hpp>

// business code headers
#include </home/tommaso/forked/rplsh/aux/sequential.hpp>


class generator_stage : public ff_node {
protected:
	std::unique_ptr<generator> src; 

public:
	generator_stage(size_t size, size_t dimension) : src(new generator(size, dimension)) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- generator -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class sorter_stage : public ff_node {
protected:
	sorter wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- sorter -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		std::vector<double> _in  = *((std::vector<double>*) t);
		std::vector<double>* _out  = new std::vector<double>();
		*_out = wrapper.compute(_in);
		delete ((std::vector<double>*) t);
		return (void*) _out;
	}
};

class multiplier_stage : public ff_node {
protected:
	multiplier wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- multiplier -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		std::vector<double> _in  = *((std::vector<double>*) t);
		std::vector<double>* _out  = new std::vector<double>();
		*_out = wrapper.compute(_in);
		delete ((std::vector<double>*) t);
		return (void*) _out;
	}
};

class sum_stage : public ff_node {
protected:
	sum wrapper; 
public:
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- sum -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		std::vector<double> _in  = *((std::vector<double>*) t);
		double* _out  = new double();
		*_out = wrapper.compute(_in);
		delete ((std::vector<double>*) t);
		return (void*) _out;
	}
};

class map0_stage : public ff_Map<std::vector<double>,std::vector<double>> {
protected:
	multiplier wrapper0;
public:
	map0_stage() : ff_Map(3) {
		pfr.disableScheduler(0);
	}

	std::vector<double>* svc(std::vector<double> *t) {
		std::vector<double>& _task = *t;
		std::vector<double>* out = &_task;
		ff_Map<std::vector<double>,std::vector<double>>::parallel_for(0, _task.size(),[this, &_task](const long i) {
			wrapper0.compute(_task[i]);
		},3);
		return out;
	}
};

class reduce0_stage : public ff_Map<std::vector<double>,double,double> {
protected:
	sum wrapper0;
public:
	reduce0_stage() : ff_Map(2) {
		pfr.disableScheduler(1);
	}

	double* svc(std::vector<double>* t) {
		std::vector<double>& _task = *t;
		double* out  = new double(wrapper0.identity);
		auto reduceF = [this](double& sum, double elem) {sum = wrapper0.compute(sum, elem);};
		auto bodyF = [this,&_task](const long i, double& sum) {sum = wrapper0.compute(sum, _task[i]);};
		ff_Map<std::vector<double>,double,double>::parallel_reduce(*out, wrapper0.identity,0,_task.size(),bodyF,reduceF,2);
		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	const char worker_mapping[] = "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15";
	threadMapper::instance()->setMappingList(worker_mapping);
    std::cout << "insert seed, size and dimension" << std::endl;
    uint seed;
    size_t size;
    int dimension;
    std::cin >> seed >> size >> dimension;

    srand (seed);

	generator_stage _generator(size, dimension);
	
	// vector of workers of farm
	std::vector<ff_node*> workers;
	
	// farm's worker 1
	sorter_stage _sorter;
	workers.push_back(&_sorter);
	
	// farm's worker 2
	sorter_stage _sorter1;
	workers.push_back(&_sorter1);
	
	// farm's worker 3
	sorter_stage _sorter2;
	workers.push_back(&_sorter2);
	
	// farm's worker 4
	sorter_stage _sorter3;
	workers.push_back(&_sorter3);
	
	// farm's worker 5
	sorter_stage _sorter4;
	workers.push_back(&_sorter4);
	
	// farm's worker 6
	sorter_stage _sorter5;
	workers.push_back(&_sorter5);
	
	// add workers to farm
	ff_farm<> farm;
	farm.add_workers(workers);
	farm.add_collector(NULL);
	
	map0_stage _map0_;
	reduce0_stage _red0_;
	ff_pipeline pipe;
	pipe.add_stage(&_generator);
	pipe.add_stage(&farm);
	pipe.add_stage(&_map0_);
	pipe.add_stage(&_red0_);
	
	
	pipe.run_and_wait_end();
	std::cout << "Spent:" << pipe.ffTime() << "msecs" << std::endl;
	
	#ifdef TRACE_FASTFLOW
	std::cout << "Stats:" << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
