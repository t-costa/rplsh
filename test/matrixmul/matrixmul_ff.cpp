// pipe(source_matrix_vectors,m,r,drain_matrixelem)

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

#include "sys/types.h"
#include "sys/sysinfo.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

struct sysinfo memInfo;

int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int getValue(){ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}


int nw = 1;
class source_matrix_vectors_stage : public ff_node {
protected:
	std::unique_ptr<source_matrix_vectors> src;

public:
	source_matrix_vectors_stage() : src(new source_matrix_vectors()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_matrix_vectors -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_matrixelem_stage : public ff_node {
protected:
	std::unique_ptr<drain_matrixelem> drn;

public:
	drain_matrixelem_stage() : drn(new drain_matrixelem()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_matrixelem -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {

		// sysinfo (&memInfo);
		// long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
		// //Add other values in next statement to avoid int overflow on right hand side...
		// virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
		// virtualMemUsed *= memInfo.mem_unit;
		//
		// std::cout << "virtual mem used: " << virtualMemUsed << std::endl;

		std::cout << "mem used: " << getValue() << std::endl;

		drn->process((utils::elem_idx_idx*) t);
		return (GO_ON);
	}
};

class map0_stage : public ff_Map<std::vector<utils::elem_elem_idx_idx>,std::vector<utils::elem_idx_idx>> {
protected:
	map_vec_prod wrapper0;
public:
	map0_stage() : ff_Map(nw) {
	}

	std::vector<utils::elem_idx_idx>* svc(std::vector<utils::elem_elem_idx_idx> *t) {
		std::vector<utils::elem_elem_idx_idx>& _task = *t;
		std::vector<utils::elem_idx_idx>* out = new std::vector<utils::elem_idx_idx>();
		out->resize(_task.size());
		size_t step = 1;
		pfr.parallel_for_static(0, _task.size(), step, 0, [this, &_task, &out, step](const long i) {
			(*out)[i] = wrapper0.op(_task[i]);
		}, nw);

		delete t;

		// std::cout << "map out:" << std::endl;
		// for (auto i=0; i<out->size(); ++i) {
		// 	std::cout << (*out)[i].a << "-" << (*out)[i].i << "-" << (*out)[i].j << std::endl;
		// }

		return out;
	}
};

class reduce0_stage : public ff_Map<std::vector<utils::elem_idx_idx>,utils::elem_idx_idx, utils::elem_idx_idx> {
protected:
	red_sum wrapper0;
public:
	reduce0_stage() : ff_Map(nw) {
	}

	utils::elem_idx_idx* svc(std::vector<utils::elem_idx_idx>* t) {
		std::vector<utils::elem_idx_idx>& _task = *t;
		utils::elem_idx_idx* out  = new utils::elem_idx_idx(wrapper0.identity);

		// std::cout << "reduce input:" << std::endl;
		// for (auto i=0; i<t->size(); ++i) {
		// 	std::cout << (*t)[i].a << "-" << (*t)[i].i << "-" << (*t)[i].j << std::endl;
		// }

		auto reduceF = [this](utils::elem_idx_idx& sum, utils::elem_idx_idx elem) {sum = wrapper0.op(sum, elem);};
		auto bodyF = [this,&_task](const long i, utils::elem_idx_idx& sum) {sum = wrapper0.op(sum, _task[i]);};
		pfr.parallel_reduce_static(*out, wrapper0.identity, 0, _task.size(), 1, -1, bodyF, reduceF, nw);

		delete t;

		// std::cout << "reduce out:" << std::endl;
		// std::cout << (*out).a << "-" << (*out).i << "-" << (*out).j << std::endl;

		return out;
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2,3,4,5,6,7";
	threadMapper::instance()->setMappingList(worker_mapping);
	while (nw <= 6) {
		source_matrix_vectors_stage _source_matrix_vectors;
		map0_stage _map0_;
		reduce0_stage _red0_;
		drain_matrixelem_stage _drain_matrixelem;
		ff_pipeline pipe;
		pipe.add_stage(&_source_matrix_vectors);
		pipe.add_stage(&_map0_);
		pipe.add_stage(&_red0_);
		pipe.add_stage(&_drain_matrixelem);


		pipe.run_and_wait_end();
		std::cout << "nw = " << nw << ". "
		;std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

		#ifdef TRACE_FASTFLOW
		std::cout << "Stats: " << std::endl;
		pipe.ffStats(std::cout);
		#endif
		if (nw < 32)
			nw++;
		else
			nw *= 2;
		}
	return 0;

}
