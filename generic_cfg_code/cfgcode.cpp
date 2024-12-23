#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

struct Rule {
    string rule;
    Rule* next;

    Rule(const string& r) : rule(r), next(nullptr) {}
};

struct Production {
    string name;
    Rule* rule_list;
    Production* next;

    Production(const string& n) : name(n), rule_list(nullptr), next(nullptr) {}
};

class CFGList {
private:
    Production* startcfg;

public:
    CFGList() : startcfg(nullptr) {}

    void addRule(Production* production, const string& rule) {
        Rule* newRule = new Rule(rule);
        if (!production->rule_list) {
            production->rule_list = newRule;
        } else {
            Rule* temp = production->rule_list;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newRule;
        }
    }

    void addProduction(const string& name) {
        Production* currentProduction = new Production(name);
        if (!startcfg) {
            startcfg = currentProduction;
        } else {
            Production* temp = startcfg;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = currentProduction;
        }
    }

    void deleteProduction(int index) {
        if (index == 0 && startcfg) {
            Production* temp = startcfg;
            startcfg = startcfg->next;
            delete temp;
        } else {
            Production* temp = startcfg;
            for (int i = 0; temp != nullptr && i < index - 1; ++i) {
                temp = temp->next;
            }
            if (temp == nullptr || temp->next == nullptr) {
                return;
            }
            Production* nextProd = temp->next->next;
            delete temp->next;
            temp->next = nextProd;
        }
    }

    void deleteRule(Production* production, int index) {
        if (index == 0 && production && production->rule_list) {
            Rule* temp = production->rule_list;
            production->rule_list = production->rule_list->next;
            delete temp;
        } else {
            Rule* temp = production->rule_list;
            for (int i = 0; temp != nullptr && i < index - 1; ++i) {
                temp = temp->next;
            }
            if (temp == nullptr || temp->next == nullptr) {
                return;
            }
            Rule* nextRule = temp->next->next;
            delete temp->next;
            temp->next = nextRule;
        }
    }

    Production* getStartCFG() {
        return startcfg;
    }

    void displayCFG() {
        Production* prodTemp = startcfg;
        int index = 0;
        while (prodTemp) {
            cout << "Production " << index << " (" << prodTemp->name << "): ";
            Rule* ruleTemp = prodTemp->rule_list;
            while (ruleTemp) {
                cout << ruleTemp->rule << " | ";
                ruleTemp = ruleTemp->next;
            }
            cout << endl;
            prodTemp = prodTemp->next;
            ++index;
        }
    }
};

void readGrammer(string filename, CFGList& cfgList){

    ifstream file;
    string line;
    file.open(filename);

    bool first = true;

    while (!file.eof()){        
        getline(file, line);
        
        stringstream lineAsStream(line);
    
        string head;
        getline(lineAsStream, head, '=');

        cfgList.addProduction(head);
        Production* nextProduction;
        if(first){
            nextProduction = cfgList.getStartCFG();
            first = false;
        }else{
            nextProduction = nextProduction->next;
        }
    
        string rule;
        while(getline(lineAsStream, rule, '|')){            
            cfgList.addRule(nextProduction, rule);
        }
    }
}

int main() {
    CFGList cfgList;

    readGrammer("test.txt",cfgList);    

    // Displaying the initial CFG
    cout << "Initial Context-Free Grammar (CFG):" << endl;
    cfgList.displayCFG();

    // Delete a rule and a production
    // cfgList.deleteRule(startProduction, 0);
    // cfgList.deleteProduction(1);

    // Displaying the updated CFG
    // cout << "\nUpdated Context-Free Grammar (CFG):" << endl;
    // cfgList.displayCFG();

    return 0;
}