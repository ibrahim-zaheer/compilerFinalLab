#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <map>

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

    map<string, set<string>> computeFirstSets() {
        map<string, set<string>> firstSets;
        Production* prodTemp = startcfg;

        // Initialize empty first sets
        while (prodTemp) {
            firstSets[prodTemp->name] = set<string>();
            prodTemp = prodTemp->next;
        }

        bool changed = true;
        while (changed) {
            changed = false;
            prodTemp = startcfg;

            while (prodTemp) {
                Rule* ruleTemp = prodTemp->rule_list;

                while (ruleTemp) {
                    stringstream ss(ruleTemp->rule);
                    string token;

                    if (getline(ss, token, ' ')) {
                        if (isupper(token[0])) { // Non-terminal
                            for (const auto& elem : firstSets[token]) {
                                if (firstSets[prodTemp->name].insert(elem).second) {
                                    changed = true;
                                }
                            }
                        } else { // Terminal
                            if (firstSets[prodTemp->name].insert(token).second) {
                                changed = true;
                            }
                        }
                    }

                    ruleTemp = ruleTemp->next;
                }

                prodTemp = prodTemp->next;
            }
        }

        return firstSets;
    }

    map<string, set<string>> computeFollowSets() {
        map<string, set<string>> followSets;
        Production* prodTemp = startcfg;

        // Initialize empty follow sets
        while (prodTemp) {
            followSets[prodTemp->name] = set<string>();
            prodTemp = prodTemp->next;
        }

        // Add "$" to the follow set of the start symbol
        if (startcfg) {
            followSets[startcfg->name].insert("$");
        }

        bool changed = true;
        while (changed) {
            changed = false;
            prodTemp = startcfg;

            while (prodTemp) {
                Rule* ruleTemp = prodTemp->rule_list;

                while (ruleTemp) {
                    stringstream ss(ruleTemp->rule);
                    string prevToken;
                    string token;

                    while (ss >> token) {
                        if (!prevToken.empty() && isupper(prevToken[0])) {
                            if (isupper(token[0])) {
                                for (const auto& elem : computeFirstSets()[token]) {
                                    if (elem != "#" && followSets[prevToken].insert(elem).second) {
                                        changed = true;
                                    }
                                }
                            } else {
                                if (followSets[prevToken].insert(token).second) {
                                    changed = true;
                                }
                            }
                        }
                        prevToken = token;
                    }

                    if (!prevToken.empty() && isupper(prevToken[0])) {
                        for (const auto& elem : followSets[prodTemp->name]) {
                            if (followSets[prevToken].insert(elem).second) {
                                changed = true;
                            }
                        }
                    }

                    ruleTemp = ruleTemp->next;
                }

                prodTemp = prodTemp->next;
            }
        }

        return followSets;
    }
};

void readGrammar(const string& filename, CFGList& cfgList) {
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream lineAsStream(line);
        string head;
        getline(lineAsStream, head, '=');
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

    readGrammar("test.txt", cfgList);

    cout << "Context-Free Grammar (CFG):" << endl;
    cfgList.displayCFG();

    auto firstSets = cfgList.computeFirstSets();
    cout << "\nFirst Sets:" << endl;
    for (const auto& [nonTerminal, firstSet] : firstSets) {
        cout << nonTerminal << " : ";
        for (const auto& symbol : firstSet) {
            cout << symbol << " ";
        }
        cout << endl;
    }

    auto followSets = cfgList.computeFollowSets();
    cout << "\nFollow Sets:" << endl;
    for (const auto& [nonTerminal, followSet] : followSets) {
        cout << nonTerminal << " : ";
        for (const auto& symbol : followSet) {
            cout << symbol << " ";
        }
        cout << endl;
    }

    return 0;
}