#ifndef rpl_rplenvironment_hpp
#define rpl_rplenvironment_hpp

#include "nodes/skeletons.hpp"
#include "environment.hpp"
#include <map>

// TODO: scatter and gather times should be something in
//       function also of the input size and the number of workers
/**
 * Extends the environment class by adding some useful getters and setters
 */
struct rpl_environment : public environment<std::string, skel_node>
{
    rpl_environment();

    void set_emitter_time( double te );
    void set_collector_time( double tc );
    void set_scatter_time( double ts );
    void set_gather_time( double tg );

    double get_emitter_time();
    double get_collector_time();
    double get_scatter_time();
    double get_gather_time();

    /// setters and getters for stream input dimension
    void set_dim( std::size_t dim );
    std::size_t get_dim();

    /// setters and getters for item input size
    void set_inputsize( std::size_t inputsize );
    std::size_t get_inputsize();

    /// setters and getters for resources
    void set_res( std::size_t res );
    std::size_t get_res();

private:
    std::map<std::string, double> sd_map;
};

#endif
