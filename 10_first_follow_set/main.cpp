#include "Production.hpp"
#include "CFG.hpp"
using namespace std;

int main()
{
     CFG cfg = CFG();
     cfg.parse_file("D:\\ibrahims academic folders\\Compiler Construction\\FinalExam\\10_first_follow_set\\input.txt");
     cout << "ORIGINAL CFG: " << endl;
     cout << cfg << endl;
     cout << "FIRST SETS: " << endl;
     map<string, set<string>> first_sets = cfg.get_first_sets();
     for (auto prod : cfg.get_productions())
     {
          auto first_set = first_sets[prod.get_name()];
          cout << prod.get_name() << " : ";
          for (string s : first_set)
          {
               cout << s << " ";
          }
          cout << endl;
     }

     cout << endl
          << "FOLLOW SETS: " << endl;
     map<string, set<string>> follow_sets = cfg.get_follow_sets();
     for (auto prod : cfg.get_productions())
     {
          auto follow_set = follow_sets[prod.get_name()];
          cout << prod.get_name() << " : ";
          for (string s : follow_set)
          {
               cout << s << " ";
          }
          cout << endl;
     }
     return 0;
}