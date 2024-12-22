#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "RecursiveDescentParser.hpp"

int main() {
    // Grammar definition
    map<string, vector<string>> grammar = {
        {"E", {"T E'"}},
        {"E'", {"+ T E'", "#"}},
        {"T", {"F T'"}},
        {"T'", {"* F T'", "#"}},
        {"F", {"( E )", "id"}}
    };

    // Input tokens to parse (example: id + id * id)
    vector<string> input = {"id", "+", "id", "*", "id", "$"};

    RecursiveDescentParser parser(input, "E", grammar);

    try {
        if (parser.parse()) {
            cout << "Input is valid according to the grammar." << endl;
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
