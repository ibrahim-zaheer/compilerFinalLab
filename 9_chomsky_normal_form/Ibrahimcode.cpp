#pragma once
#include <string>
#include <cctype>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <cerrno>
#include <cstring>
#include <iostream>
using namespace std;

namespace utils
{

    string strip_string(const string &inpt)
    {
        auto start_it = inpt.begin();
        auto end_it = inpt.rbegin();
        while (std::isspace(*start_it))
            ++start_it;
        while (std::isspace(*end_it))
            ++end_it;
        return std::string(start_it, end_it.base());
    }
}

class Rule
{
public:
    vector<string> tokenized_rule;

public:
    Rule(string rule);
    bool is_null();
    bool is_unit();
    void tokenize_rule(string rule);
    vector<string> get_tokenized_rule() const;
    const string get_rule_string() const;
    bool contains(string str);
    void remove_first_token();
    void add_token(string token);
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

const string Rule::get_rule_string() const
{
    string rule_string = "";
    for (int i = 0; i < this->tokenized_rule.size() - 1; i++)
    {
        rule_string += this->tokenized_rule[i] + " ";
    }
    rule_string += this->tokenized_rule[this->tokenized_rule.size() - 1];
    return rule_string;
}
vector<string> Rule::get_tokenized_rule() const
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

void Rule::remove_first_token()
{
    this->tokenized_rule.erase(this->tokenized_rule.begin(), this->tokenized_rule.begin() + 1);
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

void Rule::add_token(string token)
{
    this->tokenized_rule.push_back(token);
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


class Production
{
public:
    string name;
    set<Rule> rules;

public:
    Production() : name(""), rules(){};
    Production(string name) : name(utils::strip_string(name)), rules(){};
    void add_rules_from_string(string rules);
    const string get_name() const;
    void set_name(string name);
    const set<Rule> get_rules() const;
    void remove_rule(Rule rule);
    bool contains_null_rule();
    bool is_left_recursive();
    string find_unit_rule();
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

const string Production::get_name() const
{
    return this->name;
}

void Production::set_name(string name)
{
    this->name = utils::strip_string(name);
}
const set<Rule> Production::get_rules() const
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

bool Production::is_left_recursive()
{
    for (Rule r : this->rules)
    {
        if (r.get_tokenized_rule()[0] == this->name)
        {
            return true;
        }
    }
    return false;
}
string Production::find_unit_rule()
{
    for (Rule r : this->rules)
    {
        if (r.is_unit())
            return r.get_rule_string();
    }
    return "";
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


int main()
{
    CFG cfg = CFG();
    cfg.parse_file("input.txt");
    cout << "ORIGINAL CFG: " << endl;
    cout << cfg;
    cout << endl
         << "After performing first step of CNF conversion..." << endl;
    cfg.cnf_first_step();
    cout << cfg;
    cout << endl
         << "After removing null productions..." << endl;
    cfg.remove_null_productions();
    cout << cfg;
    cout << endl
         << "After removing unit productions..." << endl;
    cfg.remove_unit_productions();
    cout << cfg;
    cout << endl
         << "After performing fourth step of CNF conversion..." << endl;
    cfg.cnf_fourth_step();
    cout << cfg;
    cout << endl
         << "After performing fifth step of CNF conversion..." << endl;
    cfg.cnf_fifth_step();
    cout << cfg;

    return 0;
}