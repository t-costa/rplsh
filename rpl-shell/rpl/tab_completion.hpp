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

    static int split_string(const std::string& str, std::vector<std::string>& v);

    static inline void add_match(const std::string& word, const std::string& textstr, std::vector<std::string>& matches);

    static char* character_name_generator(const char *text, int state);

    static void match_all(const std::string& textstr, std::vector<std::string>& matches);

    static void match_identifier(const std::string& textstr, std::vector<std::string>& matches);

    static void match_env_parameters(const std::string& textstr, std::vector<std::string>& matches);

    static void match_nfun_parameters(const std::string& textstr, std::vector<std::string>& matches);

    static void match_rew_rules(const std::string& textstr, std::vector<std::string>& matches);

    static void match_opt_rules(const std::string& textstr, std::vector<std::string>& matches);

    static void match_pattern(const std::string& textstr, std::vector<std::string>& matches);

    static void match_verb(const std::string& textstr, std::vector<std::string>& matches);

    static void match_id_def(const std::string& textstr, std::vector<std::string>& matches);

    static void match_by(const std::string& textstr, std::vector<std::string>& matches);

    static void match_with(const std::string& textstr, std::vector<std::string>& matches);

    static void match_in(const std::string& textstr, std::vector<std::string>& matches);

    static void match_as(const std::string& textstr, std::vector<std::string>& matches);

    static void match_file_search(const std::string& textstr, std::vector<std::string>& matches);
};
#endif //RPL_SHELL_TAB_COMPLETION_HPP
