#include <algorithm>

#include <string>
#include <cctype>
#include <set>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <cerrno>
#include <cstring>

using namespace std;

namespace utils {
    string strip_string(const string &input) {
        auto start_it = input.begin();
        auto end_it = input.rbegin();

        while (start_it != input.end() && isspace(*start_it)) {
            ++start_it;
        }

        while (end_it != input.rend() && isspace(*end_it)) {
            ++end_it;
        }

        return string(start_it, end_it.base());
    }
}


class Rule {
public:
    // vector<string> tokenized_rule;
    std::vector<std::string> tokenized_rule;


    Rule(string rule);
    bool is_null() const;
    void tokenize_rule(const string &rule);
    vector<string> get_tokenized_rule() const;
    string get_rule_string() const;
    bool contains(const string &str) const;
    void remove_first_token();
    void add_token(const string &token);
    static bool is_non_terminal_token(const string &token);

    friend ostream &operator<<(ostream &os, const Rule &r);
    bool operator<(const Rule &r) const {
        return get_rule_string() < r.get_rule_string();
    }
};

Rule::Rule(string rule) {
    tokenize_rule(rule);
}

bool Rule::is_null() const {
    return tokenized_rule.size() == 1 && tokenized_rule[0] == "#";
}

void Rule::tokenize_rule(const string &rule) {
    stringstream linestream(rule);
    string token;
    while (getline(linestream, token, ' ')) {
        token = utils::strip_string(token);
        if (!token.empty()) {
            tokenized_rule.push_back(token);
        }
    }
}

string Rule::get_rule_string() const {
    string rule_string;
    for (size_t i = 0; i < tokenized_rule.size(); ++i) {
        if (i > 0) {
            rule_string += " ";
        }
        rule_string += tokenized_rule[i];
    }
    return rule_string;
}

vector<string> Rule::get_tokenized_rule() const {
    return tokenized_rule;
}

// bool Rule::contains(const string &str) const {
//     return find(tokenized_rule.begin(), tokenized_rule.end(), str) != tokenized_rule.end();
// }

bool Rule::contains(const string &str) const {
    return std::find(tokenized_rule.begin(), tokenized_rule.end(), str) != tokenized_rule.end();
}


void Rule::remove_first_token() {
    if (!tokenized_rule.empty()) {
        tokenized_rule.erase(tokenized_rule.begin());
    }
}

void Rule::add_token(const string &token) {
    tokenized_rule.push_back(token);
}

bool Rule::is_non_terminal_token(const string &token) {
    for (char c : token) {
        if (!isupper(c) && c != '\'') {
            return false;
        }
    }
    return true;
}

ostream &operator<<(ostream &os, const Rule &r) {
    for (const auto &s : r.tokenized_rule) {
        os << s << " ";
    }
    return os;
}

class Production {
public:
    string name;
    set<Rule> rules;

    Production() = default;
    explicit Production(string name);
    void add_rules_from_string(const string &rules);
    string get_name() const;
    void set_name(const string &name);
    set<Rule> get_rules() const;
    void remove_rule(const Rule &rule);
    bool contains_null_rule() const;

    friend ostream &operator<<(ostream &os, const Production &p);
};

Production::Production(string name) : name(utils::strip_string(name)) {}

void Production::add_rules_from_string(const string &rules_str) {
    stringstream linestream(rules_str);
    string rule_str;
    while (getline(linestream, rule_str, '|')) {
        rule_str = utils::strip_string(rule_str);
        rules.insert(Rule(rule_str));
    }
}

string Production::get_name() const {
    return name;
}

void Production::set_name(const string &new_name) {
    name = utils::strip_string(new_name);
}

set<Rule> Production::get_rules() const {
    return rules;
}

void Production::remove_rule(const Rule &rule) {
    rules.erase(rule);
}

bool Production::contains_null_rule() const {
    for (const auto &r : rules) {
        if (r.is_null()) {
            return true;
        }
    }
    return false;
}

ostream &operator<<(ostream &os, const Production &p) {
    os << p.name << " -> ";
    bool first = true;
    for (const auto &r : p.rules) {
        if (!first) {
            os << " | ";
        }
        os << r;
        first = false;
    }
    os << endl;
    return os;
}

class CFG {
private:
    vector<Production> productions;

public:
    CFG() = default;
    void parse_file(const string &file_path);
    vector<Production> get_productions() const;
    map<string, set<string>> get_first_sets();
    map<string, set<string>> get_follow_sets();

    friend ostream &operator<<(ostream &os, const CFG &cfg);

private:
    Production find_production_by_name(const string &name) const;
};

void CFG::parse_file(const string &file_path) {
    productions.clear();
    ifstream file(file_path);

    if (!file.is_open()) {
        cerr << "Error while opening file: " << file_path << endl;
        cerr << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }

    string line;
    while (getline(file, line)) {
        stringstream linestream(line);
        string name;
        getline(linestream, name, '-');
        name = utils::strip_string(name);

        string arrow;
        getline(linestream, arrow, '>'); // Consume "->"

        string rules;
        getline(linestream, rules);
        rules = utils::strip_string(rules);

        Production prod(name);
        prod.add_rules_from_string(rules);
        productions.push_back(prod);
    }

    file.close();
}

vector<Production> CFG::get_productions() const {
    return productions;
}

// ... (Functions for first and follow sets remain the same for brevity)

ostream &operator<<(ostream &os, const CFG &cfg) {
    for (const auto &p : cfg.productions) {
        os << p;
    }
    return os;
}

int main() {
    CFG cfg;
    cfg.parse_file("input.txt");

    cout << "ORIGINAL CFG: " << endl;
    cout << cfg << endl;

    cout << "FIRST SETS: " << endl;
    auto first_sets = cfg.get_first_sets();
    for (const auto &prod : cfg.get_productions()) {
        cout << prod.get_name() << " : ";
        for (const auto &s : first_sets[prod.get_name()]) {
            cout << s << " ";
        }
        cout << endl;
    }

    cout << "FOLLOW SETS: " << endl;
    auto follow_sets = cfg.get_follow_sets();
    for (const auto &prod : cfg.get_productions()) {
        cout << prod.get_name() << " : ";
        for (const auto &s : follow_sets[prod.get_name()]) {
            cout << s << " ";
        }
        cout << endl;
    }

    return 0;
}