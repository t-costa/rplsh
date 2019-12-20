#include "history.hpp"
#include "parser/lexer.hpp"
#include <algorithm>

/**
 * Retrieves the history of the given id
 * @param id searched id in history
 * @param h history where to look for
 */
history::history( const std::string& id, const history& h ) {
    error_container err_repo;

    for (auto rit = h.commands.rbegin() ; rit != h.commands.rend(); rit++) {
        auto cmd = *rit;
        std::vector<std::string> ids;
        lexer scanner(cmd.second, err_repo);
        //TODO: shouldn't check err_repo?
        auto tok = scanner.next();
        if (tok.kind == token::word && tok.data == id) {
            while (scanner.has_next()) {
                tok = scanner.next();
                if (tok.kind == token::word)
                    ids.push_back(tok.data);
            }
            commands.push_back(cmd);
            if (ids.empty()) break;
        } else if (tok.kind == token::optimize || tok.kind == token::rewrite ||
             tok.kind == token::annotate ) {
            while (scanner.has_next() && tok.kind != token::word)
                tok = scanner.next();
            if (tok.data == id)
                commands.push_back(cmd);
        }

        if (!ids.empty()) {
            for ( const auto& rid : ids ) {
                history rh(rid, h);
                merge(rh);
            }
            break;
        }
    }
}

/**
 * Adds line as the last command
 * @param line command to be inserted
 */
void history::add( const std::string& line ) {
    std::size_t index = commands.size();
    commands.emplace_back( index, line );
}

/**
 * Merges in this the content of the commands of h
 * @param h other history
 */
void history::merge(const history& h) {
    commands.insert( commands.end(), h.commands.begin(), h.commands.end() );
    std::sort(commands.begin(), commands.end(), [](const pair<std::size_t, std::string> &a, const pair<std::size_t, std::string> &b){
        return a.first < b.first;
    });
    std::unique(commands.begin(), commands.end(), [](const pair<std::size_t, std::string> &a, const pair<std::size_t, std::string> &b){
        return a.first == b.first;
    });
}

/**
 * Prints content of history
 * @param reverse flag that indicates the order of the print
 */
void history::print( bool reverse ) const {
    if (reverse)
        for (auto rit = commands.rbegin(); rit != commands.rend(); rit++)
            std::cout << rit->first << "\t" << rit->second << std::endl;
    else
        for (const auto & command : commands)
            std::cout << command.first << "\t" << command.second << std::endl;
}
