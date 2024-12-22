#pragma once

#include <set>
#include <string>
#include <sstream>
#include "utils.cpp"
#include "Rule.hpp"

using namespace std;

class Production
{
private:
    string name;
    set<Rule> rules;

public:
    Production() : name(""), rules(){};
    void add_rules_from_string(string rules);
    const string get_name();
    void set_name(string name);
    const set<Rule> get_rules();
    void remove_rule(Rule rule);
    bool contains_null_rule();
    friend ostream &operator<<(ostream &os, const Production &r);
};

void Production::add_rules_from_string(string rules)
{
    stringstream linestream(rules);
    string rule_str;
    while (getline(linestream, rule_str, '|'))
    {
        rule_str = utils::strip_string(rule_str);
        Rule rule(rule_str);
        this->rules.insert(rule);
    }
}

const string Production::get_name()
{
    return this->name;
}

void Production::set_name(string name)
{
    this->name = name;
}
const set<Rule> Production::get_rules()
{
    return this->rules;
}

void Production::remove_rule(Rule rule)
{
    auto it = this->rules.find(rule);
    if (it != this->rules.end())
    {
        this->rules.erase(it);
    }
}

bool Production::contains_null_rule()
{
    for (Rule r : this->rules)
    {
        if (r.is_null())
        {
            return true;
        }
    }
    return false;
}

ostream &operator<<(ostream &os, const Production &p)
{
    os << p.name << " -> ";
    for (Rule r : p.rules)
    {
        os << r << " | ";
    }
    os << endl;
    return os;
}