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
    friend ostream &operator<<(ostream &os, const CFG &cfg);

private:
    void replace_null_productions(Production &production);
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
            replace_null_productions(*prod_it);
        }
        prod_it--;
    }
    for (auto it = this->productions.begin(); it != this->productions.end(); it++)
    {
        it->remove_rule(Rule("#"));
    }
}

void CFG::replace_null_productions(Production &production)
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