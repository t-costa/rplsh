#ifndef rpl_set_dispatcher_hpp
#define rpl_set_dispatcher_hpp

#include "parameters.hpp"
#include "dispatcher.hpp"
#include "environment/rpl_environment.hpp"
#include <functional>

/**
 * Dispatcher for functions: for each desired information (emitter time, scatter time...)
 * stores the corresponding set function of the environmet
 */
struct set_dispatcher : public dispatcher<std::string, std::function<void (double)>>
{
    explicit set_dispatcher(rpl_environment& env) {
        add(par::emitter_time,   [&]( double value ) { env.set_emitter_time(value); });
        add(par::collector_time, [&]( double value ) { env.set_collector_time(value); });
        add(par::scatter_time,   [&]( double value ) { env.set_scatter_time(value); });
        add(par::gather_time,    [&]( double value ) { env.set_gather_time(value); });
        add(par::resources,      [&]( double value ) { env.set_res(value); });
        add(par::dimension,      [&]( double value ) { env.set_dim(value); });
        add(par::inputsize,      [&]( double value ) { env.set_inputsize(value); });
        //add(par::arch,           [&]( double value ) { env.set_emitter_time(value); });
    }
};

#endif
