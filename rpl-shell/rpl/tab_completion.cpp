//
// Created by tommaso on 2/25/20.
//

#include "tab_completion.hpp"

static std::vector<std::string> identifiers;


char** tab_completion::character_name_completion(const char *text, int start, int end) {
    //rl_attempted_completion_over = 1; //Commented because I need also the directory completion
    return rl_completion_matches(text, &tab_completion::character_name_generator);
}

void tab_completion::add_id(const std::string &id) {
    identifiers.emplace_back(id);
}

void tab_completion::split_string(const std::string& str, std::vector<std::string>& v) {
    // Used to split string around spaces.
    std::istringstream ss(str);

    // Traverse through all words
    do {
        // Read a word
        std::string word;
        ss >> word;

        v.push_back(word);

        // While there is more to read
    } while (ss);
}

inline void tab_completion::add_match(const std::string& word, const std::string& textstr, std::vector<std::string>& matches) {
    if (word.size() >= textstr.size() &&
        word.compare(0, textstr.size(), textstr) == 0) {
        matches.push_back(word);
    }
}

char* tab_completion::character_name_generator(const char *text, int state){
    static std::vector<std::string> matches;
    static size_t match_index = 0;
    std::vector<std::string> splitted;

    std::string textstr = std::string(text);
    split_string(rl_line_buffer, splitted);
    if (state == 0) {
        // During initialization, compute the actual matches for 'text' and keep
        // them in a static vector.
        matches.clear();
        match_index = 0;

        // Collect a vector of matches: vocabulary words that begin with text.
        for (const auto& word : verbs) {
            add_match(word, textstr, matches);
        }
        for (const auto& word : patterns) {
            add_match(word, textstr, matches);
        }
        for (const auto& word : refactoring_rules) {
            add_match(word, textstr, matches);
        }
        for (const auto& word : optimization_rules) {
            add_match(word, textstr, matches);
        }
        for (const auto& word : non_functional_parameters) {
            add_match(word, textstr, matches);
        }
        for (const auto& word : environmental_parameters) {
            add_match(word, textstr, matches);
        }
        for (const auto& word : identifiers) {
            add_match(word, textstr, matches);
        }

//            auto ids = m_env->get_keys();
//            for (const auto& word : ids) {
//                add_match(word, textstr, matches);
//            }
    }

    if (match_index >= matches.size()) {
        // We return nullptr to notify the caller no more matches are available.
        return nullptr;
    } else {
        // Return a malloc'd char* for the match. The caller frees it.
        return strdup(matches[match_index++].c_str());
    }
}