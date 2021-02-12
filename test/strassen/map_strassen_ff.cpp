// pipe(source_strassen,dc,drain_strassen)

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


int nw = 1;
class source_strassen_stage : public ff_node {
protected:
	std::unique_ptr<source_strassen> src;

public:
	source_strassen_stage() : src(new source_strassen()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_strassen -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_strassen_stage : public ff_node {
protected:
	std::unique_ptr<drain_strassen> drn;

public:
	drain_strassen_stage() : drn(new drain_strassen()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_strassen -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((utils::Result*) t);
		return (GO_ON);
	}
};

class map0_stage : public ff_Map<utils::Operand,utils::Result> {
protected:
	dc_strassen wrapper0;
public:
	map0_stage() : ff_Map(nw) {
	}

	utils::Result* svc(utils::Operand *t) {
		utils::Operand& _task = *t;
		utils::Result* out = new utils::Result();
		out->resize(_task.size());
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			utils::Operand tmp;
			for (size_t j=0; j<step && (i+j)<_task.size(); ++j) {
				tmp.push_back(_task[i+j]);
			}
			auto partial = wrapper0.compute(tmp);
			for (size_t j=0; j<step &&(i+j)<_task.size(); ++j) {
				(*out)[i+j] = partial[j];
			}
		}, nw);

		delete t;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2,3,4";
	threadMapper::instance()->setMappingList(worker_mapping);
	std::vector<std::pair<int, double>> par_time;
	std::vector<int> par_degree;
	while (nw <= 4) {
	source_strassen_stage _source_strassen;
	map0_stage _map0_;
	drain_strassen_stage _drain_strassen;
	ff_pipeline pipe;
	pipe.add_stage(&_source_strassen);
	pipe.add_stage(&_map0_);
	pipe.add_stage(&_drain_strassen);


	pipe.run_and_wait_end();
	std::cout << "nw = " << nw << ". ";
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	par_time.emplace_back(nw, pipe.ffTime());
	par_degree.push_back(nw);
	nw *= 2;
	}

	// utils::build_json(0, par_time, par_degree, "pipe(source_strassen,dc,drain_strassen)");
	return 0;

}
