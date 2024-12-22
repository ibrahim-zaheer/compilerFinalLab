#include "Production.hpp"
#include "CFG.hpp"
using namespace std;

int main()
{
    CFG cfg = CFG();
    cfg.parse_file("D:\\ibrahims academic folders\\Compiler Construction\\FinalExam\\6_unit_productions\\cfg.txt");
    cout << cfg;
    cfg.remove_null_productions();
    cfg.remove_unit_productions();
    cout << cfg;
    return 0;
}