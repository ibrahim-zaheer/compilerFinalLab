
#pragma once
#include <vector>
#include <string>
#include <sstream>
#include "Rule.hpp"

using namespace std;

class Production
{
protected:
    string name;
    vector<Rule> rules;

public:
    Production() : name(""), rules(){};
    void add_rules(string rules);
    const string get_name();
    void set_name(string name);
    const vector<Rule> get_rules();
    void remove_direct_left_recursion();
    friend ostream &operator<<(ostream &os, const Production &r);
};

void Production::add_rules(string rules)
{
    stringstream linestream(rules);
    string rule;
    while (getline(linestream, rule, '|'))
    {
        Rule r = Rule(rule);
        this->rules.push_back(r);
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

const vector<Rule> Production::get_rules()
{
    return this->rules;
}

void Production::remove_direct_left_recursion()
{
    vector<Rule> alpha_rules; // Rules with left recursion (A -> A alpha)
    vector<Rule> beta_rules;  // Rules without left recursion (A -> beta)

    for (const Rule &rule : rules)
    {
        const vector<string> &tokens = rule.get_tokenized_rule();
        if (!tokens.empty() && tokens[0] == name)
        {
            // Left-recursive rule
            vector<string> alpha(tokens.begin() + 1, tokens.end());
            stringstream alpha_stream;
            for (const string &token : alpha)
                alpha_stream << token << " ";
            alpha_rules.emplace_back(alpha_stream.str());
        }
        else
        {
            // Non-left-recursive rule
            beta_rules.push_back(rule);
        }
    }

    if (!alpha_rules.empty())
    {
        string new_name = name + "'"; // New non-terminal for left recursion removal
        vector<Rule> new_rules;

        // Update beta rules to include new non-terminal
        for (const Rule &beta_rule : beta_rules)
        {
            vector<string> tokens = beta_rule.get_tokenized_rule();
            stringstream updated_rule;
            for (const auto &token : tokens)
                updated_rule << token << " ";
            updated_rule << new_name;
            new_rules.emplace_back(updated_rule.str());
        }

        // Create new rules for the new non-terminal
        vector<Rule> new_alpha_rules;
        for (const Rule &alpha_rule : alpha_rules)
        {
            vector<string> tokens = alpha_rule.get_tokenized_rule();
            stringstream updated_rule;
            for (const auto &token : tokens)
                updated_rule << token << " ";
            updated_rule << new_name;
            new_alpha_rules.emplace_back(updated_rule.str());
        }
        new_alpha_rules.emplace_back("ε"); // Add epsilon (empty string) rule

        // Update current production rules
        rules = new_rules;

        // Create a new production for the new non-terminal
        Production new_production;
        new_production.set_name(new_name);
        new_production.rules = new_alpha_rules;
    }
}

ostream &operator<<(ostream &os, const Production &p)
{
    for (Rule r : p.rules)
    {
        os << p.name << " -> ";
        os << r << endl;
    }
    return os;
}

