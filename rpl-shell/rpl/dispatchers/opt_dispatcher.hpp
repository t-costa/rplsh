#ifndef rpl_opt_dispatcher_hpp
#define rpl_opt_dispatcher_hpp

#include "parameters.hpp"
#include "dispatcher.hpp"
#include "environment/rpl_environment.hpp"
#include "visitors/optimizers/optimizers.hpp"
#include <memory>

/**
 * For each optimization rule, stores the unique pointer to the corresponding
 * rule node
 */
struct opt_dispatcher : public dispatcher<std::string, std::unique_ptr<optrule>>
{
    explicit opt_dispatcher(rpl_environment& env) {
        add(par::farmopt,      std::unique_ptr<optrule>( new farmopt(env) ));
        add(par::mapopt,       std::unique_ptr<optrule>( new mapopt(env)));
        add(par::reduceopt,    std::unique_ptr<optrule>( new reduceopt(env)));
        add(par::dcopt,        std::unique_ptr<optrule>( new dcopt(env)));
        add(par::pipeopt,      std::unique_ptr<optrule>( new pipeopt(env) ));
        add(par::maxresources, std::unique_ptr<optrule>( new maxresources(env)));
        add(par::twotier,      std::unique_ptr<optrule>( new twotier(env)));
        add(par::farmfarmopt,  std::unique_ptr<optrule>( new farmfarmopt(env)));
        add(par::mapmapopt,    std::unique_ptr<optrule>( new mapmapopt(env)));
    }
};

#endif
