#ifndef rpl_cpp_parser_hpp
#define rpl_cpp_parser_hpp

#include <string>
#include <vector>
#include <utility>

struct wrapper_info;

///////////////////////////////////////////////////////////////////////////////
//
//  Parser of business logic code written in C++
//
///////////////////////////////////////////////////////////////////////////////
struct cpp_parser {
public:
    typedef std::vector<wrapper_info>::iterator iterator;
    explicit cpp_parser(const std::string& path);
    std::pair<iterator,iterator> parse();
private:
    std::vector<wrapper_info> vec;
    const std::string path;
};

///////////////////////////////////////////////////////////////////////////////
//
//  Class that provides info
//
///////////////////////////////////////////////////////////////////////////////
struct wrapper_info {
public:
    typedef enum {
        seq,
        map,
        reduce,
        source,
        drain
    } wrapper_type;

    wrapper_info(const std::string& name, wrapper_type wtype,
        const std::string& typein, const std::string& typeout);

    std::string name;
    wrapper_type wtype;
    std::string typein;
    std::string typeout;
};

#endif
