//
// Created by tommaso on 2/25/20.
//

#include <algorithm>
#include <fstream>
#include "tab_completion.hpp"

static std::unordered_set<std::string> identifiers;


char** tab_completion::character_name_completion(const char *text, int start, int end) {
    //if not commented, it's not searching on filesystem
    rl_attempted_completion_over = 1; //Commented because I need also the directory completion
    return rl_completion_matches(text, &tab_completion::character_name_generator);
}

void tab_completion::add_id(const std::string &id) {
    identifiers.insert(id);
}

/**
 *
 * @param str
 * @param v
 * @return the number of the expected word, first, second, third...
 */
int tab_completion::split_string(const std::string& str, std::vector<std::string>& v) {

    int end_pos = 0;
    //TODO: se inizia con una sequenza di spazi mi fotte tutto!
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
    //incremento sia se c'è stato un nuovo spazio (inizio n+1 parola)
    //sia se non c'è stato (deve finire la parola n)
    end_pos++;
    return end_pos;

//    // Used to split string around spaces.
//    std::istringstream ss(str);
//
//    // Traverse through all words
//    do {
//        // Read a word
//        std::string word;
//        ss >> word;
//
//        v.push_back(word);
//
//        // While there is more to read
//    } while (ss);
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

void tab_completion::match_nfun_parameters(const std::string &textstr, std::vector<std::string> &matches) {
    //TODO: qui in realtà datap non va bene per show!
    for (const auto& word : non_functional_parameters) {
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

void tab_completion::match_id_def(const std::string& textstr, std::vector<std::string>& matches) {
    for (const auto& word : id_def) {
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

void tab_completion::match_file_search(const std::string& textstr, std::vector<std::string>& matches) {
    add_match("\"", textstr, matches);
}

void complete_assignment(const std::string& textstr, std::vector<std::string>& matches) {

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

//    std::cout << "stringa test -> " << test << std::endl;
//    std::cout << "lunghezza test -> " << test.size() << std::endl;
//    std::cout << pos_seq << std::endl;
//    std::cout << pos_drain << std::endl;
//    std::cout << pos_source << std::endl;


    if (pos_seq != std::string::npos) {
        //std::cout << "è seq!" << std::endl;
        return true;
    }
    if (pos_drain != std::string::npos) {
        //std::cout << "è drain!" << std::endl;
        return true;
    }
    if (pos_source != std::string::npos) {
        //std::cout << "è source!" << std::endl;
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
        if (rl_end == 0 || (count_word <= 1 && textstr.find('=') != std::string::npos)) {
            //FIXME: io posso comunque scrivere a= ... e in quel caso
            //  voglio pattern, non verbi!
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

                if (splitted[0] == "show" || splitted[0] == "annotate") {
                    switch (count_word) {
                        case 0 :
                        case 1 :
                        case 2 :
                            match_identifier(textstr, matches);
                            break;
                        case 3 :
                            if (splitted[0] == "show") {
                                match_by(textstr, matches);
                            } else {
                                match_with(textstr, matches);
                            }
                            break;
                        default:
                            //TODO: per show va eliminato il datap
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
                    /*
                     * TODO:
                     *  seq, drain e source -> vogliono solo numeri dentro, quindi non vanno più completati
                     *  comp, farm, pipe, map, reduce -> dentro ci possono essere:
                     *      1 o più id
                     *      1 o più pattern (con regole come sopra)
                     *      miscuglio dei due
                     *      mai vuoti!
                     *  penso che i pattern vogliano sempre le (), quindi forse conviene averceli già
                     *  con le parentesi tra le opzioni...
                     * */

                } else {
                    //probably error or ill formatted, match all
                    match_all(textstr, matches);
                }
            }
        }

        /**
         *  first word  |  second word  |  rest
         *  ---------------------------------------------------
         *  annotate    |  identifier   |  with parameter value
         *  rewrite     |  identifier   |  with rewrules
         *  optimize    |  identifier   |  with optrules
         *  expand      |  identifier   |  [in identifier]
         *  add         |  identifier   |  in identifier
         *  show        |  identifier   |  [by parameters]
         *  gencode     |  identifier   | [as name] [as name in directory] [in directory]
         *  ---------------------------------------------------
         *  import      |  "file"
         *  load        |  "file"       |  boolean
         *  ---------------------------------------------------
         *  set         |  parameter    |  with number
         *  history     |  [            |  identifier]
         *  identifier  |  =            |  pattern [patterns/identifiers]
         * */
    }

    if (match_index >= matches.size()) {
        // We return nullptr to notify the caller no more matches are available.
        return nullptr;
    } else {
        // Return a malloc'd char* for the match. The caller frees it.
        return strdup(matches[match_index++].c_str());
    }
}