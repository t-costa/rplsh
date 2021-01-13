//
// Created by tommaso on 2/24/20.
//

#ifndef RPL_SHELL_NAMES_COMPLETION_HPP
#define RPL_SHELL_NAMES_COMPLETION_HPP

#include <unordered_set>
#include <string>

const std::unordered_set<std::string> verbs = {
    "show",
    "set",
    "annotate",
    "rewrite",
    "optimize",
    "history",
    "import",
    "gencode",
    "expand",
    "add",
    "load"
};

const std::unordered_set<std::string> patterns = {
    "seq(",
    "source(",
    "drain(",
    "comp(",
    "pipe(",
    "farm(",
    "map(",
    "reduce(",
    "divide_conquer("
};

const std::unordered_set<std::string> refactoring_rules {
    "farmintro",
    "farmelim",
    "pipeintro",
    "pipeelim",
    "pipeassoc",
    "compassoc",
    "mapofcomp",
    "compofmap",
    "mapofpipe",
    "pipeofmap",
    "mapelim",
    "reduceelim",
    "mapmapelim",
    "farmfarmelim",
    "compdel",
    "pipedel",
    "dctomap",
    "maptodc",
    "allrules",
    "rec"
};

const std::unordered_set<std::string> optimization_rules {
    "farmopt",
    "pipeopt",
    "mapopt",
    "reduceopt",
    "dcopt",
    "maxresources",
    "twotier",
    "farmfarmopt",
    "mapmapopt"
};

const std::unordered_set<std::string> show_parameters {
    "servicetime",
    "latency",
    "pardegree",
    "compltime",
    "resources"
};

const std::unordered_set<std::string> non_functional_parameters {
    "servicetime",
    "latency",
    "pardegree",
    "compltime",
    "resources",
    "datap",
    "grain",
    "dc_capable"
};

const std::unordered_set<std::string> grain_parameters {
    "static",
    "dynamic"
};

const std::unordered_set<std::string> environmental_parameters {
    "emitter_time",
    "collector_time",
    "scatter_time",
    "gather_time",
    "dimension",
    "inputsize",
    "resources"
};

#endif //RPL_SHELL_NAMES_COMPLETION_HPP
