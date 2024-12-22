#include "Production.hpp"
#include "CFG.hpp"
using namespace std;

int main()
{
    CFG cfg = CFG();
    cfg.parse_file("D:\\ibrahims academic folders\\Compiler Construction\\FinalExam\\5_null_productions\\cfg.txt");
    cout << cfg;
    cfg.remove_null_productions();
    cout << cfg;
    return 0;
}