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

/*
class generator_stage : public ff_node {
protected:
	std::unique_ptr<generator> src;

public:
	generator_stage(int size, int dimension) : src(new generator(size, dimension)) {}
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
			wrapper0.op(_task[i]);
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
		if (create_output_buffer(100) != 0) std::cout << "err with out";
	}

	double* svc(std::vector<double>* t) {
		std::vector<double>& _task = *t;
		double* out  = new double(wrapper0.identity);
		auto reduceF = [this](double& sum, double elem) {sum = wrapper0.op(sum, elem);};
		auto bodyF = [this,&_task](const long i, double& sum) {sum = wrapper0.op(sum, _task[i]);};
		ff_Map<std::vector<double>,double,double>::parallel_reduce(*out, wrapper0.identity,0,_task.size(),bodyF,reduceF,2);
		return out;
	}
};
*/

class generator_stage : public ff_node {
protected:
	std::unique_ptr<generator> src;

public:
	generator_stage(int size, int dimension) : src(new generator(size, dimension)) {}
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
		//std::cout << "called multiplier svc" << std::endl;
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
		//std::cout << "called sum svc" << std::endl;
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
		//std::cout << "called map svc" << std::endl;
		std::vector<double>& _task = *t;
		std::vector<double>* out = &_task;
		ff_Map<std::vector<double>,std::vector<double>>::parallel_for(0, _task.size(),[this, &_task](const long i) {
			_task[i] = wrapper0.op(_task[i]);
		},1);
		return out;
	}
};

class reduce0_stage : public ff_Map<std::vector<double>,double,double> {
protected:
	sum wrapper0;
public:
	reduce0_stage() : ff_Map(3) {
		pfr.disableScheduler(1);
	}

	double* svc(std::vector<double>* t) {
		//std::cout << "called red svc" << std::endl;
		std::vector<double>& _task = *t;
		double* out  = new double(wrapper0.identity);
		auto reduceF = [this](double& sum, double elem) {sum = wrapper0.op(sum, elem);};
		auto bodyF = [this,&_task](const long i, double& sum) {sum = wrapper0.op(sum, _task[i]);};
		ff_Map<std::vector<double>,double,double>::parallel_reduce(*out, wrapper0.identity,0,_task.size(),bodyF,reduceF,1);
		std::cout << "PAR REDUCE: out = " << *out << std::endl;
		return out;
	}
};
