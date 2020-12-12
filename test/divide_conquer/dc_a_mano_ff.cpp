// pipe(source_unbalanced_vec_stage,m1,drain_vec_stage)

#include <iostream>
#include <vector>

// specify include directory for fastflow
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include <ff/map.hpp>
#include <ff/pipeline.hpp>
#include <ff/parallel_for.hpp>
//new include
#include <ff/dc.hpp>


// specify include directory for RPL-Shell
#include <aux/types.hpp>
#include <aux/wrappers.hpp>
#include <aux/ff_comp.hpp>

// business code headers
#include </home/tommaso/forked/rplsh/test/divide_conquer/definition.hpp>


class source_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_stage> src;

public:
	source_stage_stage() : src(new source_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
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
		std::cout << "svc_init -- drain_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((utils::elem_type*) t);
		return (GO_ON);
	}
};

//non penso serva
class dc_seq_stage : public ff_node {
protected:
  dc_double_double_stage wrapper;
public:
  int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- dc_double_double_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

  void * svc(void *t) {
    //type_in _in = *(type_in *) t
    utils::elem_type _in = *((utils::elem_type*) t);
    //type_out* _out = new type_out()
    utils::elem_type* _out = new utils::elem_type();

    *_out = wrapper.compute(_in);

    delete (utils::elem_type*) t;
    return (void *) _out;
  }
};

//type_in, type_out
class dc0_stage : public ff_DC<utils::elem_type, utils::elem_type> {
protected:
  dc_double_double_stage wrapper0;
public:
  //costruttore con: divide, combine, seq, cond, n_worker
  dc0_stage() : ff_DC(
              [&](const utils::elem_type& in, std::vector<utils::elem_type>& in_vec) {
                  wrapper0.divide(in, in_vec);
                  // utils::waste(parameters::minimum_wait);
                  // in_vec.emplace_back(in - 1);
                  // in_vec.emplace_back(in - 2);
              },
              [&](std::vector<utils::elem_type>& out_vec, utils::elem_type& out) {
                wrapper0.combine(out_vec, out);
                // utils::waste(parameters::minimum_wait);
                // out = out_vec[0] + out_vec[1];
              },
              [&](const utils::elem_type& in, utils::elem_type& out) {
                wrapper0.seq(in, out);
                // utils::waste(parameters::minimum_wait);
                // out = 1;
              },
              [&](const utils::elem_type& in) {
                return wrapper0.cond(in);
                // return in <= 2;
              }, 1) {}
};


// class map0_stage : public ff_Map<std::vector<utils::elem_type>,std::vector<utils::elem_type>> {
// protected:
// 	map_minus_stage wrapper0;
// public:
// 	map0_stage() : ff_Map(4) {
// 	}
//
// 	std::vector<utils::elem_type>* svc(std::vector<utils::elem_type> *t) {
// 		std::vector<utils::elem_type>& _task = *t;
// 		std::vector<utils::elem_type>* out = &_task;
// 		pfr.parallel_for_static(0, _task.size(), 1, 0, [this, &_task, &out](const long i) {
// 			(*out)[i] = wrapper0.op(_task[i]);
// 		},4);
//
// 		return out;
// 	}
// };

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2,3,4,5,6,7";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_stage_stage _source_stage;
	// map0_stage _map0_;
  // dc0_stage _dc0_;
  dc_seq_stage _dc0_;
	drain_stage_stage _drain_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_stage);
	pipe.add_stage(&_dc0_);
	pipe.add_stage(&_drain_stage);


	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}
