/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
 */

/**
  Fibonacci: computes the n-th number of the fibonacci sequence using the ff_DAC pattern
  */

#include <iostream>
#include <functional>
#include <vector>
#include <ff/ff.hpp>

#include <ff/dc.hpp>
#include <ff/pipeline.hpp>

#include <aux/types.hpp>
#include <aux/wrappers.hpp>
#include <aux/ff_comp.hpp>

// business code headers
#include </home/tommaso/forked/rplsh/test/divide_conquer/definition.hpp>

using namespace ff;
using namespace std;

/*
 * Operand and Result are just integers
 */
using Problem = long;
using Result  = long;

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

class dc0_stage : public ff_DC<utils::elem_type, utils::elem_type> {
// protected:
public:
  dc0_stage() : ff_DC(
              [&](const utils::elem_type& in, std::vector<utils::elem_type>& in_vec) {
                  wrapper0.divide(in, in_vec);
              },
              [&](std::vector<utils::elem_type>& out_vec, utils::elem_type& out) {
                wrapper0.combine(out_vec, out);
              },
              [&](const utils::elem_type& in, utils::elem_type& out) {
                wrapper0.seq(in, out);
              },
              [&](const utils::elem_type& in) {
                return wrapper0.cond(in);
              }, 1) {}

              dc_double_double_stage wrapper0;


};

int main(int argc, char *argv[]) {
    long start = 20;
    long nwork = 4;
    if (argc>1) {
        if(argc<3){
            fprintf(stderr,"Usage: %s <N> <pardegree>\n",argv[0]);
            return -1;
        }
        start=atoi(argv[1]);
        nwork=atoi(argv[2]);
    }

	long res;
  dc_double_double_stage wrapper0;

	//lambda version
	ff_DC<utils::elem_type, utils::elem_type> dac(
    [&](const utils::elem_type& in, std::vector<utils::elem_type>& in_vec) {
        wrapper0.divide(in, in_vec);
    },
    [&](std::vector<utils::elem_type>& out_vec, utils::elem_type& out) {
      wrapper0.combine(out_vec, out);
    },
    [&](const utils::elem_type& in, utils::elem_type& out) {
      wrapper0.seq(in, out);
    },
    [&](const utils::elem_type& in) {
      return wrapper0.cond(in);
    }, 1);
	ffTime(START_TIME);
  source_stage_stage _source_stage;
  drain_stage_stage _drain_stage;
  dc0_stage _dc0_;

  ff_pipeline pipe;
  pipe.add_stage(&_source_stage);
  pipe.add_stage(&_dc0_);
  pipe.add_stage(&_drain_stage);

  pipe.run_and_wait_end();

	//compute
	// if (dac.run_and_wait_end()<0) {
  //       error("running dac");
  //       return -1;
  //   }
	ffTime(STOP_TIME);
	printf("Result: %ld\n",res);
	printf("Time (usecs): %g\n",ffTime(GET_TIME));
    return 0;
}
