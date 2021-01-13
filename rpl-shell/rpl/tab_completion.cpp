//
// Created by tommaso on 2/25/20.
//

#include <algorithm>
#include <fstream>
#include "tab_completion.hpp"

static std::unordered_set<std::string> identifiers;


char** tab_completion::character_name_completion(const char *text, int start, int end) {
    //if not commented, it's not searching on filesystem
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, &tab_completion::character_name_generator);
}

void tab_completion::add_id(const std::string &id) {
    identifiers.insert(id);
}

/**
 * Divides a string in all his words
 * @param str string to be divided
 * @param v vector where to put the found words
 * @return the number of the expected word, first, second, third...
 */
int tab_completion::split_string(const std::string& str, std::vector<std::string>& v) {

    int end_pos = 0;
    std::string word;
    for (auto c : str) {
        if (c == ' ') {
            v.push_back(word);
            word = "";
            end_pos++;
        } else {
            word += c;
        }
    }

    if (!word.empty()) {
        v.push_back(word);
    }

    //incremented when there has been a new space (start of n+1 word)
    //and also if there was no space (ending of nth word)
    end_pos++;
    return end_pos;
}

inline void tab_completion::add_match(const std::string& word, const std::string& textstr, std::vector<std::string>& matches) {
    if (word.size() >= textstr.size() &&
        word.compare(0, textstr.size(), textstr) == 0) {
        matches.push_back(word);
    }
}

void tab_completion::match_all(const std::string& textstr, std::vector<std::string>& matches) {
    match_identifier(textstr, matches);
    match_opt_rules(textstr, matches);
    match_rew_rules(textstr, matches);
    match_env_parameters(textstr, matches);
    match_nfun_parameters(textstr, matches);
    match_pattern(textstr, matches);
    match_verb(textstr, matches);
}

void tab_completion::match_identifier(const std::string& textstr, std::vector<std::string>& matches) {
    for (const auto& word : identifiers) {
        add_match(word, textstr, matches);
    }
}

void tab_completion::match_env_parameters(const std::string& textstr, std::vector<std::string>& matches) {
    for (const auto& word : environmental_parameters) {
        add_match(word, textstr, matches);
    }
}

void tab_completion::match_show_parameters(const std::string &textstr, std::vector<std::string> &matches) {
    for (const auto& word : show_parameters) {
        add_match(word, textstr, matches);
    }
}

void tab_completion::match_nfun_parameters(const std::string &textstr, std::vector<std::string> &matches) {
    for (const auto& word : non_functional_parameters) {
        add_match(word, textstr, matches);
    }
}

void tab_completion::match_grain_parameters(const std::string &textstr, std::vector<std::string> &matches) {
    for (const auto& word : grain_parameters) {
        add_match(word, textstr, matches);
    }
}

void tab_completion::match_rew_rules(const std::string& textstr, std::vector<std::string>& matches) {
    for (const auto& word : refactoring_rules) {
        add_match(word, textstr, matches);
    }
}

void tab_completion::match_opt_rules(const std::string& textstr, std::vector<std::string>& matches) {
    for (const auto& word : optimization_rules) {
        add_match(word, textstr, matches);
    }
}

void tab_completion::match_pattern(const std::string& textstr, std::vector<std::string>& matches) {
    for (const auto& word : patterns) {
        add_match(word, textstr, matches);
    }
}

void tab_completion::match_verb(const std::string& textstr, std::vector<std::string>& matches) {
    for (const auto& word : verbs) {
        add_match(word, textstr, matches);
    }
}

void tab_completion::match_with(const std::string& textstr, std::vector<std::string>& matches) {
    add_match("with", textstr, matches);
}

void tab_completion::match_by(const std::string& textstr, std::vector<std::string>& matches) {
    add_match("by", textstr, matches);
}

void tab_completion::match_in(const std::string& textstr, std::vector<std::string>& matches) {
    add_match("in", textstr, matches);
}

void tab_completion::match_as(const std::string &textstr, std::vector<std::string> &matches) {
    add_match("as", textstr, matches);
}

bool check_leaf_pattern() {
    std::string test(rl_line_buffer);

    test.erase(std::remove_if(test.begin(), test.end(),
                              [](unsigned char x) {
                                  return std::isspace(x);
                              }), test.end());

    auto pos_seq = test.find("seq", std::max((size_t) 0, test.size() - 4));
    auto pos_drain = test.find("drain", std::max((size_t) 0, test.size() - 6));
    auto pos_source = test.find("source", std::max((size_t) 0, test.size() - 7));

    if (pos_seq != std::string::npos) {
        return true;
    }
    if (pos_drain != std::string::npos) {
        return true;
    }
    if (pos_source != std::string::npos) {
        return true;
    }

    return false;
}

char* tab_completion::character_name_generator(const char *text, int state){
    static std::vector<std::string> matches;
    static size_t match_index = 0;

    if (state == 0) {
        // During initialization, compute the actual matches for 'text' and keep
        // them in a static vector.
        matches.clear();
        match_index = 0;
        std::vector<std::string> splitted;

        std::string textstr = std::string(text);
        int count_word = split_string(rl_line_buffer, splitted);

        /**
         *  first word  |  second word  |  rest
         *  ---------------------------------------------------
         *  annotate    |  identifier   |  with parameter value
         *  rewrite     |  identifier   |  with rewrules
         *  optimize    |  identifier   |  with optrules
         *  expand      |  identifier   |  [in identifier]
         *  add         |  identifier   |  in identifier
         *  show        |  identifier   |  [by parameters]
         *  gencode     |  identifier   |  [as name] [as name in directory] [in directory]
         *  ---------------------------------------------------
         *  import      |  "file"
         *  load        |  "file"       |  boolean
         *  ---------------------------------------------------
         *  set         |  parameter    |  with number
         *  history     |  [            |  identifier]
         *  identifier  |  =            |  pattern [patterns/identifiers]
         * */

        //don't know why, but with textstr.find a case such as "a=" will not work
        if (rl_end == 0 || (count_word <= 1 && splitted[0].find('=') == std::string::npos)) {
            //no input or partial input, it's looking for the first word
            match_verb(textstr, matches);
            match_identifier(textstr, matches);
        } else {
            //understand the context from the first(ish) word
            if (verbs.find(splitted[0]) != verbs.end()) {
                if (splitted[0] == "import" || splitted[0] == "load") {
                    //default behaviour
                    rl_attempted_completion_over = 0;
                }

                if (splitted[0] == "history") {
                    match_identifier(textstr, matches);
                }

                if (splitted[0] == "gencode") {
                    //based on where we are in the command, complete what is needed
                    switch (count_word) {
                        case 0:
                        case 1:
                        case 2:
                            match_identifier(textstr, matches);
                            break;
                        case 3:
                            //can be both as or in
                            match_as(textstr, matches);
                            match_in(textstr, matches);
                            break;
                        case 4:
                            if (splitted[2] == "in") {
                                rl_attempted_completion_over = 0;
                            }
                            //if it was as, it's a new name, can't complete
                            break;
                        case 5:
                            //can only be the directory
                            match_in(textstr, matches);
                            break;
                        case 6:
                            rl_attempted_completion_over = 0;
                            break;
                        default:
                            break;
                    }
                }

                if (splitted[0] == "set") {
                    switch (count_word) {
                        case 0 :
                        case 1 :
                        case 2 :
                            match_env_parameters(textstr, matches);
                            break;
                        case 3 :
                            match_with(textstr, matches);
                            break;
                        default:
                            break;
                    }
                }

                if (splitted[0] == "show") {
                    switch (count_word) {
                        case 0 :
                        case 1 :
                        case 2 :
                            match_identifier(textstr, matches);
                            break;
                        case 3 :
                            match_by(textstr, matches);
                            break;
                        default:
                            match_show_parameters(textstr, matches);
                            break;
                    }
                }

                if (splitted[0] == "annotate") {
                    switch (count_word) {
                        case 0 :
                        case 1 :
                        case 2 :
                            match_identifier(textstr, matches);
                            break;
                        case 3 :
                            match_with(textstr, matches);
                            break;
                        case 4:
                            match_nfun_parameters(textstr, matches);
                            break;
                        case 5:
                            //number || bool
                            break;
                        case 6:
                            if (splitted[3] == "grain") {
                                match_as(textstr, matches);
                            }
                            break;
                        case 7:
                            if (splitted[3] == "grain") {
                                match_grain_parameters(textstr, matches);
                            }
                            break;
                        default:
                            match_nfun_parameters(textstr, matches);
                            break;
                    }
                }

                if (splitted[0] == "expand" || splitted[0] == "add") {
                    switch (count_word) {
                        case 0 :
                        case 1 :
                        case 2 :
                            match_identifier(textstr, matches);
                            break;
                        case 3 :
                            match_in(textstr, matches);
                            break;
                        default:
                            match_identifier(textstr, matches);
                            break;
                    }
                }

                if (splitted[0] == "rewrite" || splitted[0] == "optimize") {
                    switch (count_word) {
                        case 0 :
                        case 1 :
                        case 2 :
                            match_identifier(textstr, matches);
                            break;
                        case 3 :
                            match_with(textstr, matches);
                            break;
                        default:
                            if (splitted[0] == "rewrite") {
                                match_rew_rules(textstr, matches);
                            } else {
                                match_opt_rules(textstr, matches);
                            }
                            break;
                    }
                }
            } else {
                //it's not a verb, can be an identifier or error
                if (identifiers.find(splitted[0]) != identifiers.end() ||
                    std::string(rl_line_buffer).find('=') != std::string::npos) {
                    //it's an assignment with a new or old identifier

                    if (!check_leaf_pattern()) {
                        match_pattern(textstr, matches);
                        match_identifier(textstr, matches);
                    }
                    //else it's seq/source/drain, nothing inside
                } else {
                    //probably error or ill formatted, match all
                    match_all(textstr, matches);
                }
            }
        }
    }

    if (match_index >= matches.size()) {
        // We return nullptr to notify the caller no more matches are available.
        return nullptr;
    } else {
        // Return a malloc'd char* for the match. The caller frees it.
        return strdup(matches[match_index++].c_str());
    }
}