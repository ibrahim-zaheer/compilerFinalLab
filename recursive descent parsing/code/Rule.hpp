#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "utils.cpp"
using namespace std;

class Rule
{
private:
    vector<string> tokenized_rule;

public:
    Rule(string rule);
    void tokenize_rule(string rule);
    vector<string> get_tokenized_rule();
    const string get_rule_string() const;
    static bool is_non_terminal_token(string token);
    friend ostream &operator<<(ostream &os, const Rule &r);
    bool operator<(const Rule &r) const
    {
        return this->get_rule_string() < r.get_rule_string();
    }
};

Rule::Rule(string rule)
{
    this->tokenize_rule(rule);
}

void Rule::tokenize_rule(string rule)
{
    stringstream linestream(rule);
    string token;
    while (getline(linestream, token, ' '))
    {
        token = utils::strip_string(token);
        this->tokenized_rule.push_back(token);
    }
}

const string Rule::get_rule_string() const
{
    string rule_string = "";
    for (string token : this->tokenized_rule)
    {
        rule_string += token + " ";
    }
    return rule_string;
}
vector<string> Rule::get_tokenized_rule()
{
    return this->tokenized_rule;
}

bool Rule::is_non_terminal_token(string token)
{
    for (char c : token)
    {
        if (!(c >= 'A' && c <= 'Z'))
            if (c != '\'')
                return false;
    }
    return true;
}
ostream &
operator<<(ostream &os, const Rule &r)
{
    for (string s : r.tokenized_rule)
    {
        os << s << " ";
    }
    return os;
}