// pipe(source_ordered_vec_stage,d,d1,d2,d3,d4,d5,drain_ordered_vec_stage)

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


class source_ordered_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<source_ordered_vec_stage> src;

public:
	source_ordered_vec_stage_stage() : src(new source_ordered_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- source_ordered_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		while( src->has_next() )
			ff_send_out((void*) src->next() );
		return (NULL);
	}
};

class drain_ordered_vec_stage_stage : public ff_node {
protected:
	std::unique_ptr<drain_ordered_vec_stage> drn;

public:
	drain_ordered_vec_stage_stage() : drn(new drain_ordered_vec_stage()) {}
	int svc_init() {
		#ifdef TRACE_CORE
		std::cout << "svc_init -- drain_ordered_vec_stage -- id = "		<< get_my_id() << " -- tid = " << std::this_thread::get_id() << " -- core = " << sched_getcpu() << std::endl;
		#endif
		return 0;
	}

	void * svc(void *t) {
		drn->process((std::vector<utils::elem_type>*) t);
		return (GO_ON);
	}
};

int main( int argc, char* argv[] ) {
	// worker mapping
	const char worker_mapping[] = "0,1,2,3,4,5,6,7";
	threadMapper::instance()->setMappingList(worker_mapping);
	source_ordered_vec_stage_stage _source_ordered_vec_stage;

	//default
	dc_dummy dc_stage;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage.combine(out_vec, out);
		},
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			dc_stage.seq(in, out);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return dc_stage.cond(in);
		},
	1);

	//cutoff
	dc_dummy dc_stage1;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_1(
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			dc_stage1.divide(in, in_vec);
		},
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			dc_stage1.combine(out_vec, out);
		},
		//sequential case function
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			auto in_arg = in;
			out = dc_stage1.compute(in_arg);
		},
		[&](const std::vector<utils::elem_type>& in) {
			return in.size() <= 10;
		},
	1);
	//transf, default
	map_vec_vec_stage dc_stage2;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_2(
		//divide function
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			size_t schedule = 2;
			auto new_size = in.size() / schedule;
			in_vec.resize(schedule);
			//tie scheduling
			size_t j = 0;
			for (size_t i=0; i<in.size(); ++i) {
				if (i >= (j+1)*new_size && j<schedule-1)
					j++;
				in_vec[j].push_back(in[i]);
			}
		},
		//combine function
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			size_t schedule = 2;
			size_t final_size = 0;
			//compute size of out vector
			for (size_t k=0; k<schedule; ++k) {
				final_size += out_vec[k].size();
			}
			out.resize(final_size);
			//combine results
			size_t i = 0, j = 0;
			// tie merge
			for (j=0; j<schedule; ++j) {
				for(auto& a : out_vec[j]) {
					out[i] = a;
					i++;
				}
			}
		},
		//sequential case function
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			auto in_arg = in;
			out = dc_stage2.compute(in_arg);
		},
		//condition function
		[&](const std::vector<utils::elem_type>& in) {
			return in.size() <= 1;
		},
	1);

	//transf, tie
	map_vec_vec_stage dc_stage3;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_3(
		//divide function
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			size_t schedule = 3;
			auto new_size = in.size() / schedule;
			in_vec.resize(schedule);
			//tie scheduling
			size_t j = 0;
			for (size_t i=0; i<in.size(); ++i) {
				if (i >= (j+1)*new_size && j<schedule-1)
					j++;
				in_vec[j].push_back(in[i]);
			}
		},
		//combine function
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			size_t schedule = 3;
			size_t final_size = 0;
			//compute size of out vector
			for (size_t k=0; k<schedule; ++k) {
				final_size += out_vec[k].size();
			}
			out.resize(final_size);
			//combine results
			size_t i = 0, j = 0;
			// tie merge
			for (j=0; j<schedule; ++j) {
				for(auto& a : out_vec[j]) {
					out[i] = a;
					i++;
				}
			}
		},
		//sequential case function
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			auto in_arg = in;
			out = dc_stage3.compute(in_arg);
		},
		//condition function
		[&](const std::vector<utils::elem_type>& in) {
			return in.size() <= 1;
		},
	1);

	//transf, zip
	map_vec_vec_stage dc_stage4;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_4(
		//divide function
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			size_t schedule = 3;
			in_vec.resize(schedule);
			//zip scheduling
			for (size_t i=0; i<in.size(); ++i) {
				in_vec[i % schedule].push_back(in[i]);
			}
		},
		//combine function
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			size_t schedule = 3;
			size_t final_size = 0;
			//compute size of out vector
			for (size_t k=0; k<schedule; ++k) {
				final_size += out_vec[k].size();
			}
			out.resize(final_size);
			//combine results
			size_t i = 0, j = 0;
			// zip merge
			while (i < final_size) {
				out[i] = out_vec[i % schedule][j];
				i++;
				if (i % schedule == 0) j++;
			}
		},
		//sequential case function
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			auto in_arg = in;
			out = dc_stage4.compute(in_arg);
		},
		//condition function
		[&](const std::vector<utils::elem_type>& in) {
			return in.size() <= 1;
		},
	1);
	map_vec_vec_stage dc_stage5;
	ff_DC<std::vector<utils::elem_type>, std::vector<utils::elem_type>> _dc0_5(
		//divide function
		[&](const std::vector<utils::elem_type>& in, std::vector<std::vector<utils::elem_type>>& in_vec) {
			size_t schedule = 2;
			auto new_size = in.size() / schedule;
			in_vec.resize(schedule);
			//tie scheduling
			size_t j = 0;
			for (size_t i=0; i<in.size(); ++i) {
				if (i >= (j+1)*new_size && j<schedule-1)
					j++;
				in_vec[j].push_back(in[i]);
			}
		},
		//combine function
		[&](std::vector<std::vector<utils::elem_type>>& out_vec, std::vector<utils::elem_type>& out) {
			size_t schedule = 2;
			size_t final_size = 0;
			//compute size of out vector
			for (size_t k=0; k<schedule; ++k) {
				final_size += out_vec[k].size();
			}
			out.resize(final_size);
			//combine results
			size_t i = 0, j = 0;
			// tie merge
			for (j=0; j<schedule; ++j) {
				for(auto& a : out_vec[j]) {
					out[i] = a;
					i++;
				}
			}
		},
		//sequential case function
		[&](const std::vector<utils::elem_type>& in, std::vector<utils::elem_type>& out) {
			auto in_arg = in;
			out = dc_stage5.compute(in_arg);
		},
		//condition function
		[&](const std::vector<utils::elem_type>& in) {
			return in.size() <= 15;
		},
	1);
	drain_ordered_vec_stage_stage _drain_ordered_vec_stage;
	ff_pipeline pipe;
	pipe.add_stage(&_source_ordered_vec_stage);
	pipe.add_stage(&_dc0_);
	pipe.add_stage(&_dc0_1);
	pipe.add_stage(&_dc0_2);
	pipe.add_stage(&_dc0_3);
	pipe.add_stage(&_dc0_4);
	pipe.add_stage(&_dc0_5);
	pipe.add_stage(&_drain_ordered_vec_stage);


	pipe.run_and_wait_end();
	std::cout << "Spent: " << pipe.ffTime() << " msecs" << std::endl;

	#ifdef TRACE_FASTFLOW
	std::cout << "Stats: " << std::endl;
	pipe.ffStats(std::cout);
	#endif
	return 0;

}
