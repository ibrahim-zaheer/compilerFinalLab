
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
using namespace std;

class CFG
{
protected:
    vector<Production> productions;

public:
    CFG() : productions(){};
    void parse_file(string file_path);
    void remove_cycles();
    const vector<Production> get_productions();
    friend ostream &operator<<(ostream &os, const CFG &cfg);
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
        getline(linestream, token, ':');
        temp_production.set_name(token);
        getline(linestream, token);
        temp_production.add_rules(token);
        this->productions.push_back(temp_production);
    }
    read_file.close();
}

void CFG::remove_cycles()
{
    unordered_set<string> visited;

    for (size_t i = 0; i < productions.size(); ++i)
    {
        Production &current = productions[i];
        for (size_t j = 0; j < i; ++j)
        {
            Production &prev = productions[j];

            vector<Rule> new_rules;
            for (const Rule &rule : current.get_rules())
            {
                const vector<string> &tokens = rule.get_tokenized_rule();
                if (!tokens.empty() && tokens[0] == prev.get_name())
                {
                    for (const Rule &prev_rule : prev.get_rules())
                    {
                        vector<string> combined_tokens = prev_rule.get_tokenized_rule();
                        combined_tokens.insert(combined_tokens.end(), tokens.begin() + 1, tokens.end());

                        stringstream combined_rule;
                        for (const string &token : combined_tokens)
                            combined_rule << token << " ";
                        new_rules.emplace_back(combined_rule.str());
                    }
                }
                else
                {
                    new_rules.push_back(rule);
                }
            }
            current.rules = new_rules;
        }

        current.remove_direct_left_recursion();
        visited.insert(current.get_name());
    }
}

const vector<Production> CFG::get_productions()
{
    return this->productions;
}

ostream &operator<<(ostream &os, const CFG &cfg)
{
    for (Production p : cfg.productions)
    {
        os << p;
    }
    return os;
}
