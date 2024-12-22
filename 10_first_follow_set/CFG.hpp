#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>
#include <map>
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
    map<string, set<string>> get_first_sets();
    map<string, set<string>> get_follow_sets();
    friend ostream &
    operator<<(ostream &os, const CFG &cfg);

private:
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

map<string, set<string>> CFG::get_first_sets()
{
    map<string, set<string>> first_sets;
    // initialize map keys to empty sets
    for (Production p : productions)
    {
        first_sets[p.get_name()] = set<string>();
    }
    bool changed = true;
    // iterate till no new first set element added
    while (changed)
    {
        changed = false;
        // iterate cfg's productions
        for (Production p : productions)
        {
            // iterate rules of current production
            for (Rule r : p.get_rules())
            {
                // iterate tokens of current rule
                for (string token : r.get_tokenized_rule())
                {
                    // for non terminal token find its first set and add that
                    // iterating tokens is necessary since first token might be a non terminal,
                    // whose first set contains a null rule. In that case we need to check the next token too.
                    if (Rule::is_non_terminal_token(token))
                    {
                        // get first set of the current non-terminal token
                        set<string> first_set = first_sets[token];
                        // add all the first set elements of the non-terminal token
                        // except the null rule #
                        for (string first_set_token : first_set)
                        {
                            if (first_set_token != "#")
                            {
                                // only add if not previously added.
                                // set changed status to true
                                if (first_sets[p.get_name()].find(first_set_token) == first_sets[p.get_name()].end())
                                {
                                    first_sets[p.get_name()].insert(first_set_token);
                                    changed = true;
                                }
                            }
                        }
                        // if first set does not contain null rule #, no need to check the remaining tokens
                        // just break the loop iterating over tokens of the current rule
                        if (first_set.find("#") == first_set.end())
                        {
                            break;
                        }
                    }
                    // for terminal token
                    else
                    {
                        // if token not part of first set, add it to first set
                        // set changed status to true
                        if (first_sets[p.get_name()].find(token) == first_sets[p.get_name()].end())
                        {
                            first_sets[p.get_name()].insert(token);
                            changed = true;
                        }
                        // don't iterate remaining tokens since only first non terminal token needed to be added
                        break;
                    }
                }
            }
        }
    }
    return first_sets;
}

map<string, set<string>> CFG::get_follow_sets()
{
    map<string, set<string>> follow_sets;
    // initialize map keys to empty sets
    for (Production p : productions)
    {
        follow_sets[p.get_name()] = set<string>();
    }
    // Add $ to the follow set of the start symbol
    follow_sets[productions[0].get_name()].insert("$");
    // iterate till no new follow set element added
    bool changed = true;
    while (changed)
    {
        changed = false;
        // iterate cfg's productions
        for (Production p : productions)
        {
            // iterate rules of current production
            for (Rule r : p.get_rules())
            {
                vector<string> tokens = r.get_tokenized_rule();
                // iterate over tokens of the current rule
                for (int i = 0; i < tokens.size(); i++)
                {
                    string token = tokens[i];
                    // if not a non-terminal token, move to the next token
                    if (!Rule::is_non_terminal_token(token))
                    {
                        continue;
                    }
                    // get the follow set of the current non-terminal token
                    set<string> follow_set = follow_sets[token];
                    // iterate the remaining tokens in the rule
                    // if the next token is a non-terminal, add its first set to the follow set
                    // if first set of the next token contains the null rule, move to the next token
                    // if the next token is a terminal, add the next token to the follow set and break nested loop
                    for (int j = i + 1; j < tokens.size(); j++)
                    {
                        string next_token = tokens[j];
                        // if next token is a non-terminal
                        if (Rule::is_non_terminal_token(next_token))
                        {
                            // add the first set of the next token to the follow set
                            set<string> first_set = get_first_sets()[next_token];
                            // if the first set contains the null rule, don't add the null rule to follow set.
                            // Only add the remaining set elements
                            if (first_set.find("#") != first_set.end())
                            {
                                first_set.erase("#");
                                follow_set.insert(first_set.begin(), first_set.end());
                                // if next token is at the end of the rule, add the follow set of the current production to the follow set of current token
                                if (j == tokens.size() - 1)
                                {
                                    follow_set.insert(follow_sets[p.get_name()].begin(), follow_sets[p.get_name()].end());
                                }
                            }
                            // if the first set does not contain the null rule, add the complete first set to the follow set
                            // also break the loop since no need to check the remaining tokens
                            else
                            {
                                follow_set.insert(first_set.begin(), first_set.end());
                                break;
                            }
                        }
                        // if next token is a terminal
                        else
                        {
                            // add the next token to the follow set
                            follow_set.insert(next_token);
                            // break the loop since no need to check the remaining tokens
                            break;
                        }
                    }
                    // if the rule ends with a non-terminal, add the follow set of the current production to the follow set of current token
                    if (i == tokens.size() - 1)
                    {
                        follow_set.insert(follow_sets[p.get_name()].begin(), follow_sets[p.get_name()].end());
                    }
                    // if the follow set of the current non-terminal token is changed, set changed status to true
                    if (follow_sets[token] != follow_set)
                    {
                        follow_sets[token] = follow_set;
                        changed = true;
                    }
                }
            }
        }
    }
    return follow_sets;
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

ostream &operator<<(ostream &os, const CFG &cfg)
{
    for (Production p : cfg.productions)
    {
        os << p;
    }
    return os;
}