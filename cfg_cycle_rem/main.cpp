
#include "Rule.hpp"
#include "Production.hpp"
#include "CFG.hpp"
using namespace std;

int main()
{
    CFG cfg = CFG();
    cfg.parse_file("D:\\ibrahims academic folders\\Compiler Construction\\FinalExam\\cfg_cycle_rem\\cfg.txt");
    cfg.remove_cycles();
    cout << cfg;
    return 0;
}
