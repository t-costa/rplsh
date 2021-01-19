#ifndef rpl_ann_dispatcher_hpp
#define rpl_ann_dispatcher_hpp

#include "dispatcher.hpp"
#include "parameters.hpp"
#include "visitors/annotators/annotators.hpp"
#include <memory>

/**
 * Dispatcher for annotations: stores for each type of annotation
 * the unique pointer to the visitor for the annotation node
 */
struct ann_dispatcher : public dispatcher<std::string, std::unique_ptr<ann_visitor>>
{
    explicit ann_dispatcher(rpl_environment& env) {
        add(par::servicetime    ,  std::unique_ptr<ann_visitor>(new ann_servicetime(env)));
        add(par::latency        ,  std::unique_ptr<ann_visitor>(new ann_servicetime(env)));
        add(par::pardegree      ,  std::unique_ptr<ann_visitor>(new ann_pardegree(env)));
        add(par::datap          ,  std::unique_ptr<ann_visitor>(new ann_datap(env)));
        add(par::typein         ,  std::unique_ptr<ann_visitor>(new ann_typein(env)));
        add(par::typeout        ,  std::unique_ptr<ann_visitor>(new ann_typeout(env)));
        add(par::grain          ,  std::unique_ptr<ann_visitor>(new ann_grain(env)));
        add(par::step           ,  std::unique_ptr<ann_visitor>(new ann_step(env)));
        add(par::dc_capable     ,  std::unique_ptr<ann_visitor>(new ann_dc_capable(env)));
        add(par::cutoff         ,  std::unique_ptr<ann_visitor>(new ann_cutoff(env)));
        add(par::schedule       ,  std::unique_ptr<ann_visitor>(new ann_schedule(env)));
    }
};

#endif
