#ifndef rpl_parameters_hpp
#define rpl_parameters_hpp

#include <string>

/**
 * Contains all the parameter names for the optimization rules,
 * the rewriting rules and so long as constant std::string
 */
namespace par
{
    /* rewriting rules parameter names */
    const std::string farmintro      = "farmintro";
    const std::string farmelim       = "farmelim";
    const std::string pipeintro      = "pipeintro";
    const std::string pipeelim       = "pipeelim";
    const std::string pipeassoc      = "pipeassoc";
    const std::string compassoc      = "compassoc";
    const std::string mapofcomp      = "mapofcomp";
    const std::string compofmap      = "compofmap";
    const std::string mapofpipe      = "mapofpipe";
    const std::string pipeofmap      = "pipeofmap";
    const std::string mapelim        = "mapelim";
    const std::string reduceelim     = "reduceelim";
    const std::string mapmapelim     = "mapmapelim";
    const std::string farmfarmelim   = "farmfarmelim";
    const std::string compdel        = "compdel";
    const std::string pipedel        = "pipedel";

    /* optimization parameter names */
    const std::string farmopt        = "farmopt";
    const std::string pipeopt        = "pipeopt";
    const std::string mapopt         = "mapopt";
    const std::string reduceopt      = "reduceopt";
    const std::string maxresources   = "maxresources";
    const std::string twotier        = "twotier";
    const std::string farmfarmopt    = "farmfarmopt";
    const std::string mapmapopt      = "mapmapopt";

    /* non functional name parameters used for performance evaluation */
    const std::string servicetime    = "servicetime";
    const std::string efficiency     = "efficiency";
    const std::string latency        = "latency";
    const std::string compltime      = "compltime";
    const std::string pardegree      = "pardegree";
    const std::string resources      = "resources";
    const std::string datap          = "datap";
    const std::string grain          = "grain";

    /* types for seq/source/drain */
    const std::string typein         = "typein";
    const std::string typeout        = "typeout";

    /* environment variables */
    const std::string emitter_time   = "emitter_time";
    const std::string collector_time = "collector_time";
    const std::string scatter_time   = "scatter_time";
    const std::string gather_time    = "gather_time";
    const std::string dimension      = "dimension";
    const std::string inputsize      = "inputsize";
    const std::string arch           = "arch";

    /* other std::string constants */
    const std::string show_default   = "show_default";

    /* some other flags for show */
    const std::string unranked       = "unrank";
    const std::string noann          = "noann";
    const std::string full           = "full";

    /* optional flags for grain */
    const std::string static_grain   = "static";
    const std::string dynamic_grain  = "dynamic";
}

#endif
