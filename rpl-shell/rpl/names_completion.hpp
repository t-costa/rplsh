//
// Created by tommaso on 2/24/20.
//

#ifndef RPL_SHELL_NAMES_COMPLETION_HPP
#define RPL_SHELL_NAMES_COMPLETION_HPP

#include <unordered_set>
#include <string>

//static std::unordered_set<std::string> identifiers;

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
        "reduce("
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
        "pipedel"
};

const std::unordered_set<std::string> optimization_rules {
        "farmopt",
        "pipeopt",
        "mapopt",
        "reduceopt",
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
        "grain"
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

const std::unordered_set<std::string> with {
    "with"
};

const std::unordered_set<std::string> id_def {
    "=",
    "(",
    ")"
};

const std::unordered_set<std::string> file_search {
    "\""
};

const std::unordered_set<std::string> by {
    "by"
};

#endif //RPL_SHELL_NAMES_COMPLETION_HPP
