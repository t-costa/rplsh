// comp(dsrc,m,mi,ddrn)

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
#include </home/tommaso/forked/rplsh/business/dotproduct/business.hpp>


size_type ff_res = 0;

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
		while( src->has_next() ) {
			auto vp = src->next();
			//print_vec_pair(*vp);
			ff_send_out((void*) vp );
		}
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
		drn->process((size_type*) t);
		ff_res = *((size_type*) t);
		return (GO_ON);
	}
};

// class mul_stage : public ff_node {
// protected:
// 	mul wrapper; 
// public:
// 	int svc_init() {
// 		#ifdef TRACE_CORE
// 		std::cout << "svc_init -- mul -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
// 		#endif
// 		return 0;
// 	}

// 	void * svc(void *t) {
// 		vec_pair _in  = *((vec_pair*) t);
// 		vec* _out  = new vec();
// 		*_out = wrapper.compute(_in);
// 		delete ((vec_pair*) t);
// 		return (void*) _out;
// 	}
// };

// class inc_stage : public ff_node {
// protected:
// 	inc wrapper; 
// public:
// 	int svc_init() {
// 		#ifdef TRACE_CORE
// 		std::cout << "svc_init -- inc -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
// 		#endif
// 		return 0;
// 	}

// 	void * svc(void *t) {
// 		vec _in  = *((vec*) t);
// 		size_type* _out  = new size_type();
// 		*_out = wrapper.compute(_in);
// 		delete ((vec*) t);
// 		return (void*) _out;
// 	}
// };

class map0_stage : public ff_Map<vec_pair,vec> {
protected:
	mul wrapper0;
public:
	map0_stage() : ff_Map(4) {
		pfr.disableScheduler(0);
	}

	vec* svc(vec_pair *t) {
		vec_pair& _task = *t;
		vec* out = new vec();
		out->resize(_task.size());
		ff_Map<vec_pair,vec>::parallel_for(0, _task.size(),[this, &_task, &out](const long i) {
			(*out)[i] = wrapper0.op(_task[i]);
		},4);

		// std::cout << "RISULTATO MAP (PAR)" << std::endl;
		// print_vec(*out);
		return out;
	}
};

class reduce0_stage : public ff_Map<vec,size_type,size_type> {
protected:
	inc wrapper0;
public:
	reduce0_stage() : ff_Map(4) {
		pfr.disableScheduler(1);
	}

	size_type* svc(vec* t) {
		vec& _task = *t;
		size_type* out  = new size_type(wrapper0.identity);
		auto reduceF = [this](size_type& sum, size_type elem) {sum = wrapper0.op(sum, elem);};
		auto bodyF = [this,&_task](const long i, size_type& sum) {sum = wrapper0.op(sum, _task[i]);};
		ff_Map<vec,size_type,size_type>::parallel_reduce(*out, wrapper0.identity,0,_task.size(),bodyF,reduceF,4);

		// std::cout << "RISULTATO INC (PAR)" << std::endl;
		// std::cout << *out << std::endl;
		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping 
	// const char worker_mapping[] = "0,0,1,2,0,0";
	// threadMapper::instance()->setMappingList(worker_mapping);
	dsrc_stage _dsrc;
	map0_stage _map0_;
	reduce0_stage _red0_;
	ddrn_stage _ddrn;
	ff_pipeline pipe;
	pipe.add_stage(&_dsrc);
	pipe.add_stage(&_map0_);
	pipe.add_stage(&_red0_);
	pipe.add_stage(&_ddrn);
	
	
	////// sequential /////////
	dsrc _src;
    ddrn _drn;
    mul _mul;
    inc _inc;
    size_t size;

    auto t1 = aux::now();
	auto t4 = t1;
	size_type res;
	while (_src.has_next()) {
		auto tmp = _src.next();

		size = tmp->size();
		std::cout << "size: " << size << std::endl;

		auto t2 = aux::now();
		auto v1 = _mul.compute(*tmp);
		auto t3 = aux::now();
		res = _inc.compute(v1);
		t4 = aux::now();

		// compute total times
		auto tsrc = aux::time_elapsed<aux::milliseconds>(t2, t1);
		auto tmul = aux::time_elapsed<aux::milliseconds>(t3, t2);
		auto tinc = aux::time_elapsed<aux::milliseconds>(t4, t3);

		// not interesting measuring "drain time" (negligible)
		_drn.process(&res);

		std::cout << "source time :" << tsrc << std::endl;
		std::cout << "mul time :" << tmul << " (total), " << (tmul/size) << " (per item)" << std::endl;
    	std::cout << "inc time :" << tinc << " (total), " << (tinc/size) << " (per item)" << std::endl;
	}
	auto ttot = aux::time_elapsed<aux::milliseconds>(t4, t1);
    std::cout << "total time :" << ttot << std::endl;
    
	////////////////////////////


	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;
	
	assert(res == ff_res);

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats:" << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;
	
}
