#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "ParseTable.hpp"
#include "PredictiveParser.hpp"
using namespace std;

int main()
{
    ParseTable pt("D:\\ibrahims academic folders\\Compiler Construction\\FinalExam\\Predictive Parsing Algo\\code\\table.csv");
    vector<string> input;
    // Read input from stdin
    string line;
    while (getline(cin, line))
    {
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, ' '))
        {
            cell = utils::strip_string(cell);
            if (cell != "")
            {
                input.push_back(cell);
            }
        }
    }
    input.push_back("$");
    PredictiveParser pp(input, pt);
    pp.parse();
}
