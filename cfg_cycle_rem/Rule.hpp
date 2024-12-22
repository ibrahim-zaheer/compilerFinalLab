#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include<iostream>

using namespace std;

class Rule
{
private:
    vector<string> tokenized_rule;

public:
    Rule(string rule);
    Rule() : tokenized_rule(){};
    void tokenize_rule(string rule);
    vector<string> get_tokenized_rule() const;
    friend ostream &operator<<(ostream &os, const Rule &r);
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
        this->tokenized_rule.push_back(token);
    }
}

vector<string> Rule::get_tokenized_rule() const
{
    return this->tokenized_rule;
}

ostream &operator<<(ostream &os, const Rule &r)
{
    for (string s : r.tokenized_rule)
    {
        os << s << " ";
    }
    return os;
}