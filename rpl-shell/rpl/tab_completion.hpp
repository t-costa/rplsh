//
// Created by tommaso on 2/20/20.
//

#ifndef RPL_SHELL_TAB_COMPLETION_HPP
#define RPL_SHELL_TAB_COMPLETION_HPP

//TODO: forse mi conviene mettere tutto dove stanno interpreter/parameters/history,
//  così faccio hpp/cpp e modifico il makefile corrispondente aggiungendo il cpp


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <readline/readline.h>
#include <unordered_set>
#include <environment/rpl_environment.hpp>
#include <sstream>

#include "names_completion.hpp"
//char **character_name_completion(const char *, int, int);
//char *character_name_generator(const char *, int);

struct tab_completion {
public:
    static char** character_name_completion(const char *text, int start, int end);

    static void add_id(const std::string& id);
    //explicit tab_completion(rpl_environment* t_env) : m_env(t_env) {}

private:
    //rpl_environment* m_env;

    static void split_string(const std::string& str, std::vector<std::string>& v);

    static inline void add_match(const std::string& word, const std::string& textstr, std::vector<std::string>& matches);

    static char* character_name_generator(const char *text, int state);
};
#endif //RPL_SHELL_TAB_COMPLETION_HPP
