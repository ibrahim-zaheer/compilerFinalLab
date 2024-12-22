#include <iostream>
#include <vector>
#include <string>
#include <cctype>
using namespace std;

// Function prototypes
void E();
void EPrime();
void F();
void FPrime();
void G();
void match(string expected);
void error(string message);

// Global variables
vector<string> tokens; // To store input tokens
int currentIndex = 0;  // Current token index
string callOrder = ""; // Tracks the order of function calls

// Utility to get the current token
string currentToken() {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex];
    }
    return "";
}

// Tokenizer function
vector<string> tokenize(string input) {
    vector<string> tokenList;
    string token = "";
    for (char ch : input) {
        if (isalnum(ch)) {
            token += ch; // Build multi-character tokens (e.g., "id")
        } else {
            if (!token.empty()) {
                tokenList.push_back(token);
                token = "";
            }
            if (ch == '+' || ch == '*' || ch == '$') {
                tokenList.push_back(string(1, ch));
            }
        }
    }
    if (!token.empty()) {
        tokenList.push_back(token); // Push the last token
    }
    return tokenList;
}

// Main function
int main() {
    cout << "Enter tokens (space-separated, end with $): ";
    string input;
    getline(cin, input);

    tokens = tokenize(input);

    if (tokens.empty() || tokens.back() != "$") {
        cout << "Error: Input must end with '$'" << endl;
        return 1;
    }

    try {
        E(); // Start parsing

        // Check if all tokens are consumed
        if (currentToken() == "$") {
            cout << "Parsing successful!" << endl;
            cout << "Order of function calls: " << callOrder << endl;
        } else {
            error("Unexpected tokens remaining");
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }

    return 0;
}

// Match a token
void match(string expected) {
    if (currentToken() == expected) {
        currentIndex++;
    } else {
        error("Expected '" + expected + "', but found '" + currentToken() + "'");
    }
}

// Non-terminal E → F E'
void E() {
    callOrder += "E ";
    F();
    EPrime();
}

// Non-terminal E' → + F E' | ε
void EPrime() {
    callOrder += "E' ";
    if (currentToken() == "+") {
        match("+");
        F();
        EPrime();
    }
    // ε is implicitly handled by doing nothing
}

// Non-terminal F → G F'
void F() {
    callOrder += "F ";
    G();
    FPrime();
}

// Non-terminal F' → * G F' | ε
void FPrime() {
    callOrder += "F' ";
    if (currentToken() == "*") {
        match("*");
        G();
        FPrime();
    }
    // ε is implicitly handled by doing nothing
}

// Non-terminal G → id
void G() {
    callOrder += "G ";
    if (currentToken() == "id" || isalpha(currentToken()[0])) { // Accepts any identifier
        match(currentToken());
    } else {
        error("Expected 'id'");
    }
}

// Handle errors
void error(string message) {
    throw runtime_error("Error: " + message);
}