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
    void remove_null_productions();
    void remove_unit_productions();
    void remove_left_recursion();
    friend ostream &operator<<(ostream &os, const CFG &cfg);

private:
    void add_null_production_equivalent(Production &production);
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
    auto prod_it = this->productions.begin();
    while (prod_it < this->productions.end())
    {
        string unit_rule = prod_it->find_unit_rule();
        if (unit_rule != "")
        {
            for (Production p : productions)
            {
                if (p.get_name() == unit_rule)
                {
                    prod_it->remove_rule(Rule(unit_rule));
                    prod_it->rules.insert(p.rules.begin(), p.rules.end());
                }
            }
        }
        prod_it++;
    }
}

void CFG::remove_left_recursion()
{
    // remove_null_productions();
    // remove_unit_productions();
    vector<Production> new_productions_list;
    for (Production production : productions)
    {
        if (production.is_left_recursive())
        {
            cout << "Removing left recursion for: " << production.get_name() << endl;
            Production new_production_1 = Production();
            Production new_production_2 = Production();
            new_production_1.set_name(production.get_name());
            new_production_2.set_name(production.get_name() + "'");
            for (Rule rule : production.get_rules())
            {
                // for non recursive rules
                if (rule.get_tokenized_rule()[0] != production.get_name())
                {
                    rule.add_token(production.get_name() + "'");
                    rule.get_tokenized_rule().push_back(production.get_name() + "'");
                    new_production_1.add_rules_from_string(rule.get_rule_string());
                }
                else // for recursive rules
                {
                    Rule new_rule = Rule(rule.get_rule_string());
                    new_rule.remove_first_token();
                    new_rule.add_token(production.get_name() + "'");
                    new_production_2.add_rules_from_string(new_rule.get_rule_string());
                }
            }
            cout << "New production 1: " << new_production_1 << endl;
            cout << "New production 2: " << new_production_2 << endl;
            new_production_2.add_rules_from_string("#");
            new_productions_list.push_back(new_production_1);
            new_productions_list.push_back(new_production_2);
        }
        else
        {
            new_productions_list.push_back(production);
        }
    }
    productions.clear();
    productions.assign(new_productions_list.begin(), new_productions_list.end());
}
void CFG::add_null_production_equivalent(Production &production)
{
    production.remove_rule(Rule("#"));
    string productionName = production.get_name();
    cout << "Replacing null productions for: " << productionName << endl;
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
            cout << "Replacing " << r << " with " << new_rule_str << endl;
            p.add_rules_from_string(new_rule_str);
        }
    }
}

ostream &operator<<(ostream &os, const CFG &cfg)
{
    for (Production p : cfg.productions)
    {
        os << p;
    }
    return os;
}