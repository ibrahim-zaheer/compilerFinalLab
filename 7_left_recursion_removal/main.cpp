#include "Production.hpp"
#include "CFG.hpp"
using namespace std;

int main()
{
    CFG cfg = CFG();
    cfg.parse_file("D:\\ibrahims academic folders\\Compiler Construction\\FinalExam\\7_left_recursion_removal\\cfg.txt");
    cout << cfg;
    cout << "Removing left recursion..." << endl;
    cfg.remove_left_recursion();
    cout << cfg;
    return 0;
}