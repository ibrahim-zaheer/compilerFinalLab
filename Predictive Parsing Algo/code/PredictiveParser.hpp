#pragma once
#include <stack>
#include <string>
#include <iostream>
#include <iomanip>
#include "ParseTable.hpp"
#include "Rule.hpp"

using namespace std;

class Tree
{
public:
    string value;
    vector<Tree *> children;
    Tree(string value)
    {
        this->value = value;
    }
    ~Tree()
    {
        for (auto child : children)
        {
            delete child;
        }
    }

    void print_tree_preorder(Tree *node)
    {
        if (node == nullptr)
        {
            return;
        }

        cout << node->value << endl;

        for (auto child : node->children)
        {
            print_tree_preorder(child);
        }
    }

    void print_tree()
    {
        print_tree_preorder(this);
    }
};
class PredictiveParser
{
private:
    ParseTable parse_table;
    stack<string> stack;
    vector<string> input;
    string lookahead;
    string root;

public:
    Tree *tree;

public:
    PredictiveParser(vector<string> input, ParseTable parse_table);
    void parse();
};

PredictiveParser::PredictiveParser(vector<string> input, ParseTable parse_table)
{
    this->input = input;
    this->parse_table = parse_table;
    this->root = parse_table.get_root();
    this->stack.push("$");
    this->stack.push(this->root);
}

string get_stack_string(stack<string> s)
{
    string stack_string = "";
    while (!s.empty())
    {
        stack_string += s.top() + " ";
        s.pop();
    }
    return stack_string;
}

string get_input_string(const vector<string> &input, const vector<string>::iterator &it)
{
    string input_string = "";
    int count = 0;
    for (auto i = it; i != input.end(); i++)
    {
        input_string += *i + " ";
        if (count++ > 5)
        {
            input_string += "...";
            break;
        }
    }
    return input_string;
}

void PredictiveParser::parse()
{
    cout << setw(50) << std::left << "Stack"
         << setw(50) << std::left << "Input"
         << setw(50) << std::left << "Output" << endl;
    auto cur_input = this->input.begin();
    tree = new Tree(this->parse_table.get_root());
    Tree *cur_node = tree;
    Tree *prev = tree;
    while (1)
    {
        string top = this->stack.top();
        // stack empties but input remains
        if (top == "$" && *cur_input != "$")
        {
            this->stack.push(this->root);
            continue;
        }
        // ignore null rules
        if (top == "#")
        {
            this->stack.pop();
            continue;
        }
        // input reaches end
        if (*cur_input == "$")
        {
            // both stack and input are empty
            if (this->stack.top() == "$")
            {
                cout << "FINSIHED" << endl;
                return;
            }
            // stack is not empty but input is
            else if (this->stack.top() != "#" && !Rule::is_non_terminal_token(this->stack.top()))
            {
                cout << "Error: Expected " << this->stack.top() << " but got " << *cur_input << endl;
                exit(-1);
            }
        }

        // stack top is nonterminal
        if (Rule::is_non_terminal_token(top))
        {
            // cout << "Nonterminal " << top << endl;
            string new_rule = this->parse_table.get_cell_value(top, *cur_input);
            if (new_rule == "SCAN")
            {
                cout << setw(50) << std::left << get_stack_string(this->stack)
                     << setw(50) << std::left << get_input_string(this->input, cur_input)
                     << setw(50) << std::left << "ERROR AT INPUT: " + *cur_input << endl;
                cerr << "Error: Expected " << top << " but got " << *cur_input << endl;
                cur_input++;
            }
            else if (new_rule == "POP")
            {
                cout << setw(50) << std::left << get_stack_string(this->stack)
                     << setw(50) << std::left << get_input_string(this->input, cur_input)
                     << setw(50) << std::left << "ERROR AT INPUT: " + *cur_input << endl;
                cerr << "Error: Expected " << top << " but got " << *cur_input << endl;
                this->stack.pop();
            }
            else
            { // push new rule in reverse order in stack
                this->stack.pop();
                cout << setw(50) << std::left << get_stack_string(this->stack)
                     << setw(50) << std::left << get_input_string(this->input, cur_input)
                     << setw(50) << std::left << top + " -> " + new_rule << endl;
                Rule rule(new_rule);
                vector<string> tokenized_rule = rule.get_tokenized_rule();
                cur_node = prev;
                for (auto it = tokenized_rule.rbegin(); it != tokenized_rule.rend(); it++)
                {
                    this->stack.push(*it);
                    Tree *new_node = new Tree(*it);
                    if (cur_node != nullptr)
                        cur_node->children.insert(cur_node->children.begin(), new_node);
                }
            }
        }
        else
        {
            // cout << "Terminal " << top << endl;
            if (top == *cur_input)
            {
                cout << setw(50) << std::left << get_stack_string(this->stack)
                     << setw(50) << std::left << get_input_string(this->input, cur_input)
                     << setw(50) << std::left << "Matched " + top << endl;
                this->stack.pop();
                cur_input++;
            }
            else
            {
                cout << setw(50) << std::left << get_stack_string(this->stack)
                     << setw(50) << std::left << get_input_string(this->input, cur_input)
                     << setw(50) << std::left << "ERROR AT INPUT: " + *cur_input << endl;
            }
        }
    }
}