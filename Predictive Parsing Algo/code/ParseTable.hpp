#pragma once
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <cstring>
#include "utils.cpp"

using namespace std;

class ParseTable
{
private:
    map<string, vector<string>> table;
    map<string, int> terminal_cols;
    string root;

public:
    ParseTable(){};
    ParseTable(string filename);
    const string get_cell_value(string nonterminal, string terminal);
    const string get_root() const;
    ~ParseTable();

private:
    void add_terminal_cols(string line);
};

ParseTable::ParseTable(string filname)
{
    ifstream table_file(filname);
    if (!table_file.is_open())
    {
        cerr << "Error while opening file: " << filname << endl;
        cerr << strerror(errno) << endl;
        table_file.close();
        exit(-1);
    }
    string line;
    getline(table_file, line);
    add_terminal_cols(line);
    int line_count = 0;
    while (getline(table_file, line))
    {
        stringstream ss(line);
        string cell;
        string row_name;
        int cell_count = 0;
        while (getline(ss, cell, ','))
        {
            cell = utils::strip_string(cell);
            // Check if the cell is wrapped in double quotes
            if (cell.front() == '"')
            {
                string prev_cell = cell + ",";
                getline(ss, cell, '"');
                cell = prev_cell + cell + '"';
                // Remove the quotes
                cell = cell.substr(1, cell.size() - 2);
                string temp;
                getline(ss, temp, ',');
            }
            if (cell_count == 0)
            {
                if (line_count == 0) // for root production
                    root = cell;
                row_name = cell;
                table[row_name] = vector<string>();
            }
            else
            {
                table[row_name].push_back(cell);
            }
            cell_count++;
        }
        line_count++;
        if (cell_count != terminal_cols.size())
            throw runtime_error("ParseTable: Invalid number of cells in row " + row_name + " (expected " + to_string(terminal_cols.size()) + ", got " + to_string(cell_count) + ")");
    }
}

const string ParseTable::get_cell_value(string nonterminal, string terminal)
{
    if (table.find(nonterminal) == table.end())
        throw runtime_error("ParseTable: Invalid nonterminal " + nonterminal);
    if (terminal_cols.find(terminal) == terminal_cols.end())
        throw runtime_error("ParseTable: Invalid terminal " + terminal);
    return table[nonterminal][terminal_cols[terminal] - 1];
}

const string ParseTable::get_root() const
{
    return root;
}

void ParseTable::add_terminal_cols(string line)
{
    stringstream ss(line);
    string cell;
    int cell_count = 0;
    while (getline(ss, cell, ','))
    {
        cell = utils::strip_string(cell);
        if (cell.front() == '"')
        {

            string prev_cell = cell + ",";
            getline(ss, cell, '"');
            cell = prev_cell + cell + '"';
            // Remove the quotes
            cell = cell.substr(1, cell.size() - 2);
            string temp;
            getline(ss, temp, ',');
        }
        terminal_cols[cell] = cell_count++;
    }
}

ParseTable::~ParseTable()
{
}