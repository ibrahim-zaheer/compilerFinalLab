#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// Structure to represent a Rule
struct Rule {
    vector<string> terms;
    Rule* next;  // Link to the next rule

    Rule(const vector<string>& t) : terms(t), next(nullptr) {}
};

// Structure to represent a Production
struct Prod {
    string NT;     // Non-terminal
    Rule* rules;   // Linked list of rules
    Prod* next;    // Link to the next production

    Prod(string nt) : NT(nt), rules(nullptr), next(nullptr) {}

    // Add a rule to the production
    void addRule(const vector<string>& terms) {
        Rule* newRule = new Rule(terms);
        newRule->next = rules;
        rules = newRule;
    }

    // Print all the rules of this production
    void print() {
        cout << NT << " -> ";
        Rule* rule = rules;
        while (rule) {
            for (size_t i = 0; i < rule->terms.size(); ++i) {
                cout << rule->terms[i];
                if (i != rule->terms.size() - 1)
                    cout << " ";
            }
            rule = rule->next;
            if (rule) cout << " | ";
        }
        cout << endl;
    }
};

// Function to remove unit productions
void removeUnitProductions(Prod* prods) {
    bool updated = true;
    while (updated) {
        updated = false;
        Prod* prod = prods;
        while (prod) {
            Rule* prevRule = nullptr;
            Rule* rule = prod->rules;
            while (rule) {
                if (rule->terms.size() == 1 && isupper(rule->terms[0][0])) {
                    // It's a unit production (non-terminal -> non-terminal)
                    string nonTerminal = rule->terms[0];

                    // Find the unit production's non-terminal and add its rules
                    Prod* unitProd = prods;
                    while (unitProd && unitProd->NT != nonTerminal) {
                        unitProd = unitProd->next;
                    }

                    if (unitProd) {
                        Rule* unitRule = unitProd->rules;
                        while (unitRule) {
                            vector<string> newRuleTerms = unitRule->terms;
                            Rule* newRule = new Rule(newRuleTerms);
                            newRule->next = rule->next;

                            if (prevRule) {
                                prevRule->next = newRule;
                            } else {
                                prod->rules = newRule;
                            }

                            rule = newRule;
                            unitRule = unitRule->next;
                            updated = true;
                        }
                    }
                }
                prevRule = rule;
                rule = rule->next;
            }
            prod = prod->next;
        }
    }
}

int main() {
    Prod* prods = nullptr;  // Linked list of productions
    string input;

    cout << "Enter production rules (e.g., S->a B), type 'exit' to stop:\n";

    while (true) {
        getline(cin, input);
        if (input == "exit") break;

        stringstream ss(input);
        string left, right;
        getline(ss, left, '-');
        getline(ss, right);

        // Trim the '>' character
        right = right.substr(1);

        // Add production to linked list
        Prod* newProd = new Prod(left);
        newProd->next = prods;
        prods = newProd;

        // Split the right-hand side into rules separated by '|'
        stringstream ruleStream(right);
        string rule;
        while (getline(ruleStream, rule, '|')) {
            vector<string> terms;
            for (char c : rule) {
                if (c != ' ') {
                    terms.push_back(string(1, c));
                }
            }
            newProd->addRule(terms);
        }
    }

    // Remove unit productions
    removeUnitProductions(prods);

    // Print the final productions
    cout << "\nProductions after removing unit productions:\n";
    Prod* prod = prods;
    while (prod) {
        prod->print();
        prod = prod->next;
    }

    // Clean up memory
    while (prods) {
        Prod* tempProd = prods;
        prods = prods->next;

        // Clean up all rules for the current production
        while (tempProd->rules) {
            Rule* tempRule = tempProd->rules;
            tempProd->rules = tempProd->rules->next;
            delete tempRule;
        }

        delete tempProd;
    }

    return 0;
}
