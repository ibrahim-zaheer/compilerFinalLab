#include "Production.hpp"
#include "CFG.hpp"
using namespace std;

int main()
{
    CFG cfg = CFG();
    cfg.parse_file("D:\\ibrahims academic folders\\Compiler Construction\\FinalExam\\8_indirect_left_recursion_removal\\cfg.txt");
    cout << cfg;
    auto temp = cfg.find_indirect_left_recursion("X");
    for (string t : temp)
    {
        cout << t << "->";
    }
    cout << endl;
    return 0;
}