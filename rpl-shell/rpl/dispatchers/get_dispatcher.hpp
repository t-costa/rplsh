#ifndef rpl_get_dispatcher_hpp
#define rpl_get_dispatcher_hpp

#include "parameters.hpp"
#include "dispatcher.hpp"
#include "environment/rpl_environment.hpp"
#include <functional>

/**
 * Dispatcher for functions: stores for each desired information (emitter time, scatter time,
 * resources...) the corresponding get function of the environment
 */
struct get_dispatcher : public dispatcher<std::string, std::function<double (void)>>
{
    explicit get_dispatcher(rpl_environment& env) {
        add(par::emitter_time,   [&]() { return env.get_emitter_time(); });
        add(par::collector_time, [&]() { return env.get_collector_time(); });
        add(par::scatter_time,   [&]() { return env.get_scatter_time(); });
        add(par::gather_time,    [&]() { return env.get_gather_time(); });
        add(par::resources,      [&]() { return env.get_res(); });
        add(par::dimension,      [&]() { return env.get_dim(); });
        add(par::inputsize,      [&]() { return env.get_inputsize(); });
    }
};

#endif
