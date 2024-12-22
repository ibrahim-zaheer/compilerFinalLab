#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>
#include "Production.hpp"
#include "Rule.hpp"
#include "utils.cpp"

using namespace std;

class CFG
{
private:
    vector<Production> productions;

public:
    CFG() : productions(){};
    void parse_file(string file_path);
    const vector<Production> get_productions();
    void convert_to_cnf();
    void cnf_first_step();
    void remove_null_productions();
    void remove_unit_productions();
    void cnf_fourth_step();
    void cnf_fifth_step();
    string get_new_production_name();
    friend ostream &
    operator<<(ostream &os, const CFG &cfg);

private:
    void add_null_production_equivalent(Production &production);
    Production find_production_by_name(string name);
};

void CFG::parse_file(string file_path)
{
    productions.clear();
    ifstream read_file(file_path);
    if (!read_file.is_open())
    {
        cerr << "Error while opening file: " << file_path << endl;
        cerr << strerror(errno) << endl;
        read_file.close();
        exit(-1);
    }
    string current_line = "";
    while (getline(read_file, current_line))
    {
        Production temp_production = Production();
        string token = "";
        stringstream linestream(current_line);
        getline(linestream, token, '-');
        token = utils::strip_string(token);
        temp_production.set_name(token);
        getline(linestream, token, '>');
        getline(linestream, token);
        token = utils::strip_string(token);
        temp_production.add_rules_from_string(token);
        this->productions.push_back(temp_production);
    }
    read_file.close();
}

const vector<Production> CFG::get_productions()
{
    return this->productions;
}

void CFG::convert_to_cnf()
{
    this->remove_null_productions();
    this->remove_unit_productions();
    this->cnf_first_step();
    this->cnf_fourth_step();
    this->cnf_fifth_step();
}
void CFG::cnf_first_step()
{
    string root_production_name = productions[0].get_name();
    for (Production p : productions)
    {
        for (Rule r : p.get_rules())
        {
            if (r.get_tokenized_rule().size() == 1 && r.get_tokenized_rule()[0] == root_production_name)
            {

                Production new_root_production = Production(root_production_name + "'");
                new_root_production.add_rules_from_string(root_production_name);
                this->productions.insert(this->productions.begin(), new_root_production);
                return;
            }
        }
    }
}

void CFG::remove_null_productions()
{
    auto prod_it = this->productions.end() - 1;
    while (prod_it >= this->productions.begin())
    {
        if (prod_it->contains_null_rule())
        {
            add_null_production_equivalent(*prod_it);
        }
        prod_it--;
    }
    for (auto it = this->productions.begin(); it != this->productions.end(); it++)
    {
        it->remove_rule(Rule("#"));
    }
}

void CFG::remove_unit_productions()
{
    int count = 0;
    auto prod_it = this->productions.begin();
    while (prod_it < this->productions.end())
    {
        // find the unit rule in current production
        string unit_rule = prod_it->find_unit_rule();
        while (unit_rule != "" && unit_rule != prod_it->get_name())
        {
            if (count++ > 100)
            {
                cout << "A cycle probably exists in grammar. Aborting unit rule removal." << endl;
                exit(-1);
            }
            // find full definition of the unit rule from the CFG
            // replace unit rule with its full definition
            // cout << "Found unit rule: " << unit_rule << " in production: " << prod_it->get_name() << endl;
            for (Production p : this->productions)
            {
                if (p.get_name() == unit_rule)
                {
                    prod_it->remove_rule(Rule(unit_rule));
                    prod_it->rules.insert(p.rules.begin(), p.rules.end());
                }
            }
            unit_rule = prod_it->find_unit_rule();
        }
        prod_it++;
    }
}

void CFG::cnf_fourth_step()
{
    for (int prod_idx = 0; prod_idx < productions.size(); prod_idx++)
    {
        Production &cur_prod = productions[prod_idx];
        vector<Rule> rules = vector<Rule>(cur_prod.rules.begin(), cur_prod.rules.end());
        for (int rule_idx = 0; rule_idx < rules.size(); rule_idx++)
        {
            Rule &cur_rule = rules[rule_idx];
            if (cur_rule.get_tokenized_rule().size() > 2)
            {
                // cout << "Found rule with more than 2 tokens: " << cur_rule << endl;
                string new_production_name = get_new_production_name();
                Production new_production = Production(new_production_name);
                string new_rule_1 = cur_rule.get_tokenized_rule()[0] + " " + new_production_name;
                string old_rule = cur_rule.get_rule_string();
                string new_rule_2 = string(old_rule.begin() + cur_rule.tokenized_rule[0].size(), old_rule.end());
                // cout << "New rule 1: " << new_rule_1 << endl;
                // cout << "New rule 2: " << new_rule_2 << endl;
                // modify rule in cur prod
                rules.push_back(Rule(new_rule_1));
                rules.erase(rules.begin() + rule_idx);
                rule_idx--;
                // add a new production with remaining rule part
                new_production.add_rules_from_string(new_rule_2);
                productions.push_back(new_production);
            }
        }
        // note that since we have modified the orginal productions vector
        // the vector has moved to another memory location
        // cur_prod will no longer point to actual current production in the loop
        // use prod_idx here
        productions[prod_idx].rules = set<Rule>(rules.begin(), rules.end());
    }
}

Production CFG::find_production_by_name(string name)
{
    for (Production &p : productions)
    {
        if (p.get_name() == name)
        {
            return p;
        }
    }
    throw "Production " + name + " not found in grammar.";
}

void CFG::add_null_production_equivalent(Production &production)
{
    production.remove_rule(Rule("#"));
    string productionName = production.get_name();
    // cout << "Replacing null productions for: " << productionName << endl;
    auto productoins = this->get_productions();
    for (int i = 0; i < productions.size(); i++)
    {
        Production &p = productions[i];
        for (Rule r : p.get_rules())
        {
            if (!r.contains(productionName))
                continue;
            string new_rule_str = "";
            for (string rule_token : r.get_tokenized_rule())
            {
                if (rule_token != productionName)
                {
                    new_rule_str += rule_token + " ";
                }
            }
            new_rule_str = new_rule_str == "" ? "#" : new_rule_str;
            // cout << "Replacing " << r << " with " << new_rule_str << endl;
            p.add_rules_from_string(new_rule_str);
        }
    }
}

void CFG::cnf_fifth_step()
{
    for (int prod_idx = 0; prod_idx < productions.size(); prod_idx++)
    {
        Production &cur_prod = productions[prod_idx];
        vector<Rule> rules = vector<Rule>(cur_prod.rules.begin(), cur_prod.rules.end());
        for (int rule_idx = 0; rule_idx < rules.size(); rule_idx++)
        {
            Rule &cur_rule = rules[rule_idx];
            if (cur_rule.get_tokenized_rule().size() == 2)
            {
                string first_token = cur_rule.get_tokenized_rule()[0];
                string second_token = cur_rule.get_tokenized_rule()[1];
                if (!Rule::is_non_terminal_token(first_token) && Rule::is_non_terminal_token(second_token))
                {
                    // cout << "Found rule with 1 terminal token and 1 non-terminal token: " << cur_rule << endl;
                    string new_production_name = get_new_production_name();
                    Production new_production = Production(new_production_name);
                    string new_rule_1 = new_production_name + " " + second_token;
                    string new_rule_2 = first_token;
                    // cout << "New rule 1: " << new_rule_1 << endl;
                    // cout << "New rule 2: " << new_rule_2 << endl;
                    // modify rule in cur prod
                    rules.push_back(Rule(new_rule_1));
                    rules.erase(rules.begin() + rule_idx);
                    rule_idx--;
                    // add a new production with remaining rule part
                    new_production.add_rules_from_string(new_rule_2);
                    productions.push_back(new_production);
                }
            }
        }
        // note that since we have modified the orginal productions vector
        // the vector has moved to another memory location
        // cur_prod will no longer point to actual current production in the loop
        // use prod_idx here
        productions[prod_idx].rules = set<Rule>(rules.begin(), rules.end());
    }
}

string CFG::get_new_production_name()
{
    Production p = this->productions[this->productions.size() - 1];
    string name = p.get_name();
    string new_name = string(1, name[0] + 1);
    new_name += name.substr(1, name.size() - 1);
    return new_name;
}

ostream &operator<<(ostream &os, const CFG &cfg)
{
    for (Production p : cfg.productions)
    {
        os << p;
    }
    return os;
}