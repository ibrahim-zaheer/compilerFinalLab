#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
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

    Production* getStartCFG() {
        return startcfg;
    }

    void displayCFG() {
        Production* prodTemp = startcfg;
        while (prodTemp) {
            cout << prodTemp->name << " -> ";
            Rule* ruleTemp = prodTemp->rule_list;
            while (ruleTemp) {
                cout << ruleTemp->rule;
                if (ruleTemp->next) cout << " | ";
                ruleTemp = ruleTemp->next;
            }
            cout << endl;
            prodTemp = prodTemp->next;
        }
    }

    void removeNullProductions() {
        Production* prodTemp = startcfg;

        while (prodTemp) {
            Rule* ruleTemp = prodTemp->rule_list;

            while (ruleTemp) {
                if (ruleTemp->rule == "#") {
                    Rule* temp = ruleTemp;
                    ruleTemp = ruleTemp->next;
                    delete temp;
                } else {
                    ruleTemp = ruleTemp->next;
                }
            }

            prodTemp = prodTemp->next;
        }
    }

    void removeUnitProductions() {
        Production* prodTemp = startcfg;
        while (prodTemp) {
            Rule* ruleTemp = prodTemp->rule_list;

            while (ruleTemp) {
                if (isupper(ruleTemp->rule[0]) && ruleTemp->rule.size() == 1) {
                    Production* targetProd = findProduction(ruleTemp->rule);

                    if (targetProd) {
                        Rule* targetRule = targetProd->rule_list;
                        while (targetRule) {
                            addRule(prodTemp, targetRule->rule);
                            targetRule = targetRule->next;
                        }

                        Rule* temp = ruleTemp;
                        ruleTemp = ruleTemp->next;
                        delete temp;
                    } else {
                        ruleTemp = ruleTemp->next;
                    }
                } else {
                    ruleTemp = ruleTemp->next;
                }
            }

            prodTemp = prodTemp->next;
        }
    }

    void convertToCNF() {
        removeNullProductions();
        removeUnitProductions();
        // Additional CNF steps can be added here.
    }

private:
    Production* findProduction(const string& name) {
        Production* prodTemp = startcfg;
        while (prodTemp) {
            if (prodTemp->name == name) {
                return prodTemp;
            }
            prodTemp = prodTemp->next;
        }
        return nullptr;
    }
};

void readGrammar(const string& filename, CFGList& cfgList) {
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream lineAsStream(line);
        string head;
        getline(lineAsStream, head, '-');
        cfgList.addProduction(head);

        Production* currentProduction = cfgList.getStartCFG();
        while (currentProduction->next) {
            currentProduction = currentProduction->next;
        }

        string rule;
        while (getline(lineAsStream, rule, '|')) {
            cfgList.addRule(currentProduction, rule);
        }
    }
}

int main() {
    CFGList cfgList;

    readGrammar("input.txt", cfgList);

    cout << "Original CFG:" << endl;
    cfgList.displayCFG();

    cout << "\nConverting to CNF..." << endl;
    cfgList.convertToCNF();

    cout << "\nCNF CFG:" << endl;
    cfgList.displayCFG();

    return 0;
}
