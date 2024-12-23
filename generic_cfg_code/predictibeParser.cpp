#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <iomanip>

using namespace std;

struct Rule {
    string rule;
    Rule* next;

    Rule(const string& r) : rule(r), next(nullptr) {}
};

struct Production {
    string name;
    Rule* rule_list;
    Production* next;

    Production(const string& n) : name(n), rule_list(nullptr), next(nullptr) {}
};

class ParseTable {
private:
    map<string, vector<string>> table;
    map<string, int> terminal_cols;
    string root;

public:
    ParseTable(const string& filename);
    const string get_cell_value(const string& nonterminal, const string& terminal);
    const string get_root() const;

private:
    void add_terminal_cols(const string& line);
};

ParseTable::ParseTable(const string& filename) {
    ifstream table_file(filename);
    if (!table_file.is_open()) {
        cerr << "Error while opening file: " << filename << endl;
        exit(-1);
    }

    string line;
    getline(table_file, line);
    add_terminal_cols(line);

    int line_count = 0;
    while (getline(table_file, line)) {
        stringstream ss(line);
        string cell;
        string row_name;
        int cell_count = 0;
        while (getline(ss, cell, ',')) {
            if (cell_count == 0) {
                if (line_count == 0) root = cell;
                row_name = cell;
                table[row_name] = vector<string>();
            } else {
                table[row_name].push_back(cell);
            }
            cell_count++;
        }
        line_count++;
        if (cell_count != terminal_cols.size())
            throw runtime_error("ParseTable: Invalid row size.");
    }
    table_file.close();
}

const string ParseTable::get_cell_value(const string& nonterminal, const string& terminal) {
    if (table.find(nonterminal) == table.end())
        throw runtime_error("ParseTable: Invalid nonterminal " + nonterminal);
    if (terminal_cols.find(terminal) == terminal_cols.end())
        throw runtime_error("ParseTable: Invalid terminal " + terminal);
    return table[nonterminal][terminal_cols[terminal] - 1];
}

const string ParseTable::get_root() const {
    return root;
}

void ParseTable::add_terminal_cols(const string& line) {
    stringstream ss(line);
    string cell;
    int cell_count = 0;
    while (getline(ss, cell, ',')) {
        terminal_cols[cell] = cell_count++;
    }
}

class PredictiveParser {
private:
    ParseTable parse_table;
    stack<string> stack;
    vector<string> input;
    string root;

public:
    PredictiveParser(vector<string> input, ParseTable parse_table);
    void parse();

private:
    string get_stack_string(stack<string> s);
    string get_input_string(const vector<string>& input, const vector<string>::iterator& it);
};

PredictiveParser::PredictiveParser(vector<string> input, ParseTable parse_table) {
    this->input = input;
    this->parse_table = parse_table;
    this->root = parse_table.get_root();
    this->stack.push("$");
    this->stack.push(this->root);
}

string PredictiveParser::get_stack_string(stack<string> s) {
    string stack_string;
    while (!s.empty()) {
        stack_string += s.top() + " ";
        s.pop();
    }
    return stack_string;
}

string PredictiveParser::get_input_string(const vector<string>& input, const vector<string>::iterator& it) {
    string input_string;
    for (auto i = it; i != input.end(); ++i) {
        input_string += *i + " ";
    }
    return input_string;
}

void PredictiveParser::parse() {
    cout << setw(30) << left << "Stack"
         << setw(30) << left << "Input"
         << setw(30) << left << "Output" << endl;

    auto cur_input = input.begin();

    while (true) {
        string top = stack.top();
        if (top == "$" && *cur_input == "$") {
            cout << "Parsing completed successfully." << endl;
            break;
        }

        if (top == *cur_input) {
            stack.pop();
            ++cur_input;
        } else if (isupper(top[0])) {
            string rule = parse_table.get_cell_value(top, *cur_input);
            stack.pop();
            if (rule != "#") {
                for (auto it = rule.rbegin(); it != rule.rend(); ++it) {
                    stack.push(string(1, *it));
                }
            }
        } else {
            cerr << "Error: Unexpected token " << *cur_input << " at input." << endl;
            break;
        }

        cout << setw(30) << left << get_stack_string(stack)
             << setw(30) << left << get_input_string(input, cur_input)
             << endl;
    }
}

int main() {
    ParseTable pt("table.csv");
    vector<string> input;

    cout << "Enter the input string: " << endl;
    string line;
    getline(cin, line);
    stringstream ss(line);
    string token;
    while (ss >> token) {
        input.push_back(token);
    }
    input.push_back("$");

    PredictiveParser parser(input, pt);
    parser.parse();

    return 0;
}
