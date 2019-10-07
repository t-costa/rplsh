///////////////////////////////////////////////////////////////////////////////
//
//          Main program
//
///////////////////////////////////////////////////////////////////////////////
#include "rpl/interpreter.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <locale>
#include <regex>

// for readline library
#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

rpl_environment env;                    // environment: <name, skel_tree> bindings
error_container err_repo;               // filled with errors

bool print_rpl() {
    cout << rplsh::name;
    return true;
}

bool is_empty_input(string& line) {
    return line.empty() || line == string(line.length(), ' ');
}

bool is_comment(string& line) {
    regex comment("^([ ]*)#(.*)");
    return regex_match(line, comment);
}

bool is_quit_input(string& line) {
    return line.length() == 1 && ( line[0] == 'q' || line[0] == 'Q' );
}

void process(interpreter& _interpr, string& line) {
    err_repo.reset();
    lexer _scanner(line, err_repo);
    parser _parser(_scanner, err_repo);

    unique_ptr<rpl_node> t = _parser.parse();
    if (err_repo.size() == 0)
        t->accept(_interpr);
    if (err_repo.size() == 0)
        _interpr.get_history().add(line);
    if (err_repo.size() > 0)
        cout << err_repo.get(0);
}

int main(int argc, char * argv[])
{
    string line;                                // input line
    interpreter _interpr( env, err_repo );        // interpreter

    if (argc > 1) {
        // import environment with script
        ifstream infile(argv[1]);
        while ( getline(infile, line) ) {
            print_rpl(); cout << line << endl;
            if (is_empty_input(line) || is_comment(line))
                continue;
            process(_interpr, line);
        }
    }

    char *buffer;
    while ( (buffer = readline(rplsh::name.c_str())) ) {
//        if ( !buffer ) {
//            std::cout << rplsh::name << " exit..." << std::endl;
//        }

        std::string new_line(buffer);
        add_history(new_line.c_str());
        if (is_empty_input(new_line) || is_comment(new_line))
            continue;
        if (is_quit_input(new_line))
            break;
        process(_interpr, new_line);
        free(buffer);
    }

    free(buffer);
    /*
    while ( print_rpl() && getline(cin, line) ) {
        // interactive mode
        if (is_empty_input(line) || is_comment(line))
            continue;
        if (is_quit_input(line))
            break;
        process(_interpr, line);
    }*/

    return 0;
}
