#include "Production.hpp"
#include "CFG.hpp"
using namespace std;

int main()
{
    CFG cfg = CFG();
    cfg.parse_file("D:\\ibrahims academic folders\\Compiler Construction\\FinalExam\\9_chomsky_normal_form\\input.txt");
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