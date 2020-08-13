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

#include "tab_completion.hpp"

using namespace std;

rpl_environment env;                    // environment: <name, skel_tree> bindings
error_container err_repo;               // filled with errors

void usage(const token& tok) {
    switch (tok.kind) {
        case token::word : {
            std::cout << "Assignment usage: ";
            std::cout << "<identifier> = <pattern>" << std::endl;
            break;
        }
        case token::show : {
            std::cout << "Show usage: ";
            std::cout << "show <identifier> [by <list of parameters, separated by commas>]" << std::endl;
            break;
        }
        case token::set : {
            std::cout << "Set usage: ";
            std::cout << "set <parameter> with <number>" << std::endl;
            break;
        }
        case token::annotate : {
            std::cout << "Annotate usage: ";
            std::cout << "annotate <identifier> with <parameter value>" << std::endl;
            break;
        }
        case token::rewrite : {
            std::cout << "Rewrite usage: ";
            std::cout << "rewrite <identifier> with <rewriting rules, separated by commas>" << std::endl;
            break;
        }
        case token::optimize : {
            std::cout << "Optimize usage: ";
            std::cout << "optimize <identifier> with <optimization rules, separated by commas>" << std::endl;
            break;
        }
        case token::history : {
            std::cout << "History usage: ";
            std::cout << "history [<identifier>]" << std::endl;
            break;
        }
        case token::import : {
            std::cout << "Import usage: ";
            std::cout << "import \"<path to source file>\"" << std::endl;
            break;
        }
        case token::gencode : {
            std::cout << "Gencode usage: ";
            std::cout << "gencode <identifier>" << std::endl;
            break;
        }
        case token::expand : {
            std::cout << "Expand usage: ";
            std::cout << "expand <identifier1> [in <identifier2>]" << std::endl;
            break;
        }
        case token::add : {
            std::cout << "Add usage: ";
            std::cout <<  "add <identifier1> in <identifier2>" << std::endl;
            break;
        }
        case token::load : {
            std::cout << "Load usage: ";
            std::cout << "load \"<path to commands file>\" [true/false]" << std::endl;
            break;
        }
        default :
            break;
    }
}

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

    auto t = _parser.parse();
    if (err_repo.size() == 0) {
        t.first->accept(_interpr);
    }
    //there might be an error after the accept
    if (err_repo.size() == 0){
        _interpr.get_history().add(line);
    }
    if (err_repo.size() > 0) {
        cout << err_repo.get(0);
        usage(t.second);
    }
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

    rl_attempted_completion_function = tab_completion::character_name_completion;
    char *buffer;
    while ( (buffer = readline(rplsh::name.c_str())) ) {
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

    return 0;
}
