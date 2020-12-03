#include "cpp_parser.hpp"
#include <fstream>
#include <regex>
#include <utility>

using namespace std;

/**
 * Finds a word in a string
 * @param str string in which to search
 * @return substring without spaces
 */
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}

cpp_parser::cpp_parser( string  path ) :
    path(std::move(path))
{}

/**
 * Parse source file
 * @return couple (begin, end) of the list of structures found
 */
pair<cpp_parser::iterator, cpp_parser::iterator> cpp_parser::parse() {

    // assumption: path to file exists -> there is a check before this operation
    ifstream file( path );

    // name: $3, typein: $9, typeout: $12
    regex seq_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)seq_wrapper([ ]*)<([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*)>");
    // name: $3, typeout: $9
    regex src_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)source([ ]*)<([ ]*)(.+)([ ]*)>");
    // name: $3, typein: $9
    regex drn_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)drain([ ]*)<([ ]*)(.+)([ ]*)>");
    // name: $3, typein: $9, typeout: $12, typein_el: $15, typeout_el: $18
    regex map_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)map_stage_wrapper([ ]*)<([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*)>");
    // name: $3, typein: $9, typeout: $12, typein_el: $15, typeout_el: $18
    regex red_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)reduce_stage_wrapper([ ]*)<([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*)>");
    // name: $3, typein: $9, typeout: $12
    regex dc_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)dc_stage_wrapper([ ]*)<([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*)>");

    string line;
    smatch match;

    while (std::getline(file, line)) {
        if ( std::regex_search(line, match, seq_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::seq,
                match[9], match[12]);
        } else if ( std::regex_search(line, match, src_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::source,
                "", match[9]);
        } else if ( std::regex_search(line, match, drn_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::drain,
                match[9], "");
        } else if ( std::regex_search(line, match, map_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::map,
                match[9], match[12], match[15], match[18]);
        } else if ( std::regex_search(line, match, red_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::reduce,
                match[9], match[12], match[15], match[18]);
        } else if (std::regex_search(line, match, dc_regex)) {
            vec.emplace_back(trim(match[3]), wrapper_info::dc,
                             match[9], match[12]);
        }
    }

    return {vec.begin(), vec.end()};
}

wrapper_info::wrapper_info(string  name, wrapper_type wtype,
    string  typein, string  typeout) :
    name(std::move(name)),
    wtype(wtype),
    typein(std::move(typein)),
    typeout(std::move(typeout))
{}

wrapper_info::wrapper_info(string  name, wrapper_type wtype,
                           string  typein, string  typeout,
                           string  typein_el, string  typeout_el) :
    name(std::move(name)),
    wtype(wtype),
    typein(std::move(typein)),
    typeout(std::move(typeout)),
    typein_el(std::move(typein_el)),
    typeout_el(std::move(typeout_el))
{}
