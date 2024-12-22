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
    string rule_string;

public:
    Rule(string rule);
    bool is_null();
    bool is_unit();
    void tokenize_rule(string rule);
    const vector<string> get_tokenized_rule();
    const string get_rule_string() { return rule_string; };
    bool contains(string str);
    friend ostream &operator<<(ostream &os, const Rule &r);
    bool operator<(const Rule &r) const
    {
        return this->rule_string < r.rule_string;
    }
};

Rule::Rule(string rule)
{
    this->rule_string = rule;
    this->tokenize_rule(rule);
}

bool Rule::is_null()
{
    return tokenized_rule.size() == 1 && tokenized_rule[0] == "#";
}

bool Rule::is_unit()
{
    if (tokenized_rule.size() != 1)
        return false;
    for (char c : tokenized_rule[0])
    {
        if (!isupper(c))
            return false;
    }
    return true;
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

const vector<string> Rule::get_tokenized_rule()
{
    return this->tokenized_rule;
}

bool Rule::contains(string str)
{
    for (string token : this->tokenized_rule)
    {
        if (token == str)
            return true;
    }
    return false;
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