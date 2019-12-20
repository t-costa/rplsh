#include "rpl_environment.hpp"

using namespace std;

/**
 * Fills the environment with default values
 */
rpl_environment::rpl_environment() {
    set_emitter_time(1);
    set_collector_time(1);
    set_scatter_time(1);
    set_gather_time(1);
    set_dim(1000);
    set_inputsize(1);
    set_res(16);
}

void rpl_environment::set_emitter_time(double te) {
    sd_map["te"] = te;
}

void rpl_environment::set_collector_time(double tc) {
    sd_map["tc"] = tc;
}

void rpl_environment::set_scatter_time(double ts) {
    sd_map["ts"] = ts;
}

void rpl_environment::set_gather_time(double tg) {
    sd_map["tg"] = tg;
}

double rpl_environment::get_emitter_time() {
    return sd_map["te"];
}

double rpl_environment::get_collector_time() {
    return sd_map["tc"];
}

double rpl_environment::get_scatter_time() {
    return sd_map["ts"];
}

double rpl_environment::get_gather_time() {
    return sd_map["tg"];
}

void rpl_environment::set_dim(size_t dim) {
    sd_map["dim"] = dim;
}

size_t rpl_environment::get_dim() {
    return (size_t) sd_map["dim"];
}

void rpl_environment::set_inputsize(size_t inputsize) {
    sd_map["inputsize"] = inputsize;
}

size_t rpl_environment::get_inputsize() {
    return (size_t) sd_map["inputsize"];
}

void rpl_environment::set_res(size_t res) {
    sd_map["res"] = res;
}

size_t rpl_environment::get_res() {
    return (size_t) sd_map["res"];
}
