#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Rule.hpp"

using namespace std;

class RecursiveDescentParser {
private:
    vector<string> input;  // The input tokens to parse
    size_t current_token;  // Current position in the input
    string root;           // The root non-terminal of the grammar
    map<string, vector<string>> grammar; // Grammar rules (non-terminal -> list of productions)

    string get_current_token() {
        return (current_token < input.size()) ? input[current_token] : "$"; // "$" signifies end of input
    }

    void advance_token() {
        if (current_token < input.size()) current_token++;
    }

    // Recursive functions for each non-terminal
    bool parse_rule(const string& non_terminal);

public:
    RecursiveDescentParser(vector<string> input, const string& root, map<string, vector<string>> grammar);
    bool parse();
};

RecursiveDescentParser::RecursiveDescentParser(vector<string> input, const string& root, map<string, vector<string>> grammar)
    : input(move(input)), current_token(0), root(root), grammar(move(grammar)) {}

bool RecursiveDescentParser::parse() {
    if (parse_rule(root)) {
        if (get_current_token() == "$") {
            cout << "Parsing completed successfully!" << endl;
            return true;
        } else {
            throw runtime_error("Error: Input not fully consumed. Remaining token: " + get_current_token());
        }
    }
    throw runtime_error("Parsing failed.");
}

bool RecursiveDescentParser::parse_rule(const string& non_terminal) {
    cout << "Attempting to parse rule for: " << non_terminal << endl;

    if (grammar.find(non_terminal) == grammar.end()) {
        throw runtime_error("Error: Unknown non-terminal: " + non_terminal);
    }

    vector<string> productions = grammar[non_terminal];
    for (const string& production : productions) {
        size_t saved_position = current_token;
        cout << "Testing production: " << production << " at token: " << get_current_token() << endl;
        stringstream ss(production);
        string token;
        bool success = true;

        while (ss >> token) {
            if (Rule::is_non_terminal_token(token)) {
                cout << "Recursing on non-terminal: " << token << endl;
                if (!parse_rule(token)) {
                    success = false;
                    break;
                }
            } else {
                cout << "Matching terminal: " << token << " with " << get_current_token() << endl;
                if (get_current_token() == token) {
                    advance_token();
                } else {
                    success = false;
                    break;
                }
            }
        }

        if (success) return true;

        // Backtrack to the saved position
        current_token = saved_position;
        cout << "Backtracking to position: " << saved_position << endl;
    }
    return false;
}
