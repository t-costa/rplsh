#ifndef rpl_rr_dispatcher_hpp
#define rpl_rr_dispatcher_hpp

#include "dispatcher.hpp"
#include "rewriting/rewrules.hpp"
#include "parameters.hpp"
#include <string>
#include <memory>
#include <vector>

/**
 * Dispatcher for the rewriting rules; for each rule stores the unique pointer to
 * the rule node
 */
struct rr_dispatcher : public dispatcher<std::string, std::unique_ptr<rewrule>>
{
    explicit rr_dispatcher(rpl_environment& env) {
        add(par::pipeintro , std::unique_ptr<rewrule> (new pipeintro(env)));
        add(par::pipeelim  , std::unique_ptr<rewrule> (new pipeelim(env)));
        add(par::compassoc , std::unique_ptr<rewrule> (new compassoc(env)));
        add(par::pipeassoc , std::unique_ptr<rewrule> (new pipeassoc(env)));
        add(par::mapofcomp , std::unique_ptr<rewrule> (new mapofcomp(env)));
        add(par::compofmap , std::unique_ptr<rewrule> (new compofmap(env)));
        add(par::mapofpipe , std::unique_ptr<rewrule> (new mapofpipe(env)));
        add(par::pipeofmap , std::unique_ptr<rewrule> (new pipeofmap(env)));
        add(par::mapelim   , std::unique_ptr<rewrule> (new mapelim(env)));
        add(par::reduceelim, std::unique_ptr<rewrule> (new reduceelim(env)));
        add(par::farmelim  , std::unique_ptr<rewrule> (new farmelim(env)));
        add(par::farmintro , std::unique_ptr<rewrule> (new farmintro(env)));
        
        /* the following rules are not in allrules */
        add(par::mapmapelim, std::unique_ptr<rewrule> (new mapmapelim(env)));
        add(par::farmfarmelim, std::unique_ptr<rewrule> (new farmfarmelim(env)));
        add(par::compdel, std::unique_ptr<rewrule> (new compdel(env)));
        add(par::pipedel, std::unique_ptr<rewrule> (new pipedel(env)));
        add(par::dctomap, std::unique_ptr<rewrule> (new dctomap(env)));
        add(par::maptodc, std::unique_ptr<rewrule> (new maptodc(env)));
    }

    /**
     * @return list of all the rules that are included in allrules
     */
    std::vector<std::string> get_allrules() {
        return {
            par::mapelim,
            par::pipeintro,
            par::pipeelim,
            par::compassoc,
            par::pipeassoc,
            par::mapofcomp,
            par::compofmap,
            par::mapofpipe,
            par::pipeofmap,
            par::farmelim,
            par::farmintro
        };
    }

};

#endif
