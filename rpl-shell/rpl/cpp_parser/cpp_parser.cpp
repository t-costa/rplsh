#include "cpp_parser.hpp"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

//string trim(const string& str);

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

cpp_parser::cpp_parser( const string& path ) :
    path(path)
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
    regex map_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)map_stage_wrapper([ ]*)<([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*)>");   //<([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*)>");
    // name: $3, typein: $9, typeout: $12, typein_el: $15, typeout_el: $18
    regex red_regex("(class|struct)([ ]*)(.+)([ ]*):([ ]*)public([ ]*)reduce_stage_wrapper([ ]*)<([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*),([ ]*)(.+)([ ]*)>");
    //TODO: non dovrebbe servire il tipo degli elementi interni (controllo sui tipi a comp.)

    string line;
    smatch match;

    while (std::getline(file, line)) {
        if (line == "class dsrc : public source<vec_pair> {") {
            std::cout << "ci sono " << std::endl;
        }

        if ( std::regex_search(line, match, seq_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::seq,
                match[9], match[12]);   //TODO: check, ho appena scambiato in e out
        } else if ( std::regex_search(line, match, src_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::source,
                "", match[9]);
        } else if ( std::regex_search(line, match, drn_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::drain,
                match[9], "");
        } else if ( std::regex_search(line, match, map_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::map,
                match[9], match[12]);
        } else if ( std::regex_search(line, match, red_regex) ) {
            vec.emplace_back(trim(match[3]), wrapper_info::reduce,
                match[9], match[12]);
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
