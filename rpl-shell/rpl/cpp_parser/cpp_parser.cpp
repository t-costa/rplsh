#include "cpp_parser.hpp"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

string trim(const string& str);

string trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}

cpp_parser::cpp_parser( const string& path ) :
    path(path)
{}

pair<cpp_parser::iterator, cpp_parser::iterator> cpp_parser::parse() {

    // assumption: path to file exists
    ifstream file( path );

    // name: $3, typein: $9, typeout: $12
    regex seq_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)seq_wrapper([ ]*)<([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*)>");
    // name: $3, typeout: $9
    regex src_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)source([ ]*)<([ ]*)(.+)([ ]*)>");
    // name: $3, typein: $9
    regex drn_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)drain([ ]*)<([ ]*)(.+)([ ]*)>");

    string line;
    smatch match;
    while (std::getline(file, line)) {
        if( std::regex_search(line, match, seq_regex) ) {
            vec.push_back( wrapper_info( trim(match[3]), wrapper_info::seq,
                match[12], match[9]));
        } else if( std::regex_search(line, match, src_regex) ) {
            vec.push_back( wrapper_info( trim(match[3]), wrapper_info::source,
                "", match[9]));
        } else if( std::regex_search(line, match, drn_regex) ) {
            vec.push_back( wrapper_info( trim(match[3]), wrapper_info::drain,
                match[9], ""));
        }
    }

    return {vec.begin(), vec.end()};
}

wrapper_info::wrapper_info(const string& name, wrapper_type wtype,
    const string& typein, const string& typeout) :
    name(name),
    wtype(wtype),
    typein(typein),
    typeout(typeout)
{}
