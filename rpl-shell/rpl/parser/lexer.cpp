#include "lexer.hpp"

using namespace std;

lexer::lexer (const string& line, error_container& err_repo)
    : prev(0), pos(0), line(line), err_repo(err_repo), end(false) {}

bool lexer::has_next() const {
    return !end;
}

const string& lexer::getline() const {
    return line;
}

/**
 * Checks if word is a valid token
 * @param word string to be checked
 * @param wpos position in the input line
 * @param err_repo error container
 * @return valid token if or illegal token if error
 */
token get_token(const string& word, int wpos, error_container& err_repo)
{
    token::type kind = token::st_map[word];
    smatch match;
    if (kind > 0)                                                               // keyword
        return token(kind, word, wpos);
    else if (regex_match(word, regex("[a-zA-z][a-zA-Z_0-9]*")))          // word
        return token(token::word, word, wpos);
    else if (regex_match(word, regex("([0]|[1-9][0-9]*)[.][0-9]*")))     // number
        return token(token::number, word, wpos);
    else if (regex_match(word, regex("([0]|[1-9][0-9]*)")))              // integer
        return token(token::integer, word, wpos);
    else if (regex_match(word, match, regex("\"([^\"]*)\""))) {
        return token(token::file, match[1], wpos);
    }

    // nothing found
    err_repo.add( shared_ptr<error>(new error_illegal(word, wpos)));
    return token(token::illegal, word, wpos);                      // illegal
}

/**
 * Scans the input line searching for valid tokens or eol
 * @return valid token, eol or illegal token
 */
token lexer::next() {
    string word;

    while ((pos = line.find_first_of(" ,()[]=-+", prev)) != string::npos ) {
        word = line.substr(prev, pos - prev + (pos == prev)*1 );
        int wpos = prev; prev = pos + (pos == prev)*1;
        if (!word.empty() && !isspace(word[0]))
            return get_token(word, wpos, err_repo);
    }

    // the last word
    if (pos == string::npos && prev < line.length()) {
        int wpos = prev; prev = string::npos;
        return get_token(line.substr(wpos, string::npos), wpos, err_repo);
    }

    // end has been reached and case of trailing space
    end = true;
    return token(token::eol, "", line.length());
}
