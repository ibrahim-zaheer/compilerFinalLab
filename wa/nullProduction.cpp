#include <iostream>
#include <string>
#include <string.h>
using namespace std;

class Rule
{
public:
    string terms[10];
    int num_terms;
    Rule *next;
    Rule *prev;

    Rule()
    {
        num_terms = 0;
        next = nullptr;
        prev = nullptr;
    }
};

class Prod
{
public:
    string NT;
    Prod *next;
    Prod *prev;
    Rule *first_rule;
    Rule *last_rule;
    bool has_Epsilon;
    bool isResolved;

    Prod()
    {
        NT = "";
        next = nullptr;
        prev = nullptr;
        first_rule = nullptr;
        last_rule = nullptr;
        has_Epsilon = false;
        isResolved = false;
    }
};

void addRule(Prod *prod, const string terms[], int num_terms)
{
    if (num_terms == 0)
        return; // Skip if no terms provided

    Rule *newRule = new Rule();
    newRule->num_terms = num_terms;

    for (int i = 0; i < num_terms; i++)
    {
        if (terms[i] == "^")
        {
            prod->has_Epsilon = true;
        }
        newRule->terms[i] = terms[i];
    }

    if (prod->first_rule == nullptr)
    {
        prod->first_rule = prod->last_rule = newRule;
    }
    else
    {
        prod->last_rule->next = newRule;
        newRule->prev = prod->last_rule;
        prod->last_rule = newRule;
    }
}

void removeRule(Prod *prod, const string &val)
{
    Rule *current = prod->first_rule;

    while (current != nullptr)
    {
        if (current->terms[0] == val)
        {
            if (current->prev != nullptr)
            {
                current->prev->next = current->next;
            }
            else
            {
                prod->first_rule = current->next;
            }

            if (current->next != nullptr)
            {
                current->next->prev = current->prev;
            }
            else
            {
                prod->last_rule = current->prev;
            }

            // Ensure both pointers are null if no rules remain
            if (prod->first_rule == nullptr && prod->last_rule == nullptr)
            {
                prod->first_rule = prod->last_rule = nullptr;
            }

            delete current;
            return;
        }
        current = current->next;
    }
}

// Function to add a new production to the linked list of productions
Prod *addProd(Prod **head, string non_terminal)
{
    Prod *newProd = new Prod();
    newProd->NT = non_terminal;

    if (*head == nullptr)
    {
        *head = newProd;
        return newProd;
    }
    else
    {
        Prod *temp = *head;
        while (temp->next != nullptr && temp->NT != non_terminal)
        {
            temp = temp->next;
        }
        if (temp->NT != non_terminal)
        {
            temp->next = newProd;
            newProd->prev = temp;
            return newProd;
        }
        else
        {
            return temp; // Return the existing Prod if it already exists
        }
    }
}

// Function to print a production and its rules
void printProd(Prod *prod)
{
    Rule *rule = prod->first_rule;
    cout << prod->NT << " -> ";
    while (rule != nullptr)
    {
        for (int i = 0; i < rule->num_terms; i++)
        {
            cout << rule->terms[i];
        }
        if (rule->next != nullptr)
        {
            cout << "| ";
        }
        rule = rule->next;
    }
    cout << endl;
}

// Function to traverse the linked list of productions and print them
void printProductions(Prod *head)
{
    Prod *temp = head;
    while (temp != nullptr)
    {
        printProd(temp);
        temp = temp->next;
    }
}

// Function to split the input string into non-terminal and rule parts
void splitString(const string &str, const string &delimiter, string &prodNT, string &rule)
{
    size_t pos = str.find(delimiter);
    if (pos != string::npos)
    {
        prodNT = str.substr(0, pos);
        rule = str.substr(pos + delimiter.length());
    }
    else
    {
        cout << "Delimiter not found!" << endl;
    }
}

// Function to split the rule into terms
void splitRuleIntoTerms(const string &rule, string terms[], int &num_terms)
{
    num_terms = 0; // Reset the count
    for (char c : rule)
    {
        if (c != ' ') // Ignore spaces
        {
            terms[num_terms++] = string(1, c); // Convert the character to a string and store it
        }
    }
}


void removeNullTerm(Prod *tail, string val)
{
    if (tail == nullptr)
        return;
    Prod *temp = tail;

    while (temp != nullptr)
    {
        Rule *tempRule = temp->first_rule;
        while (tempRule != nullptr)
        {
            bool flag = false;
            string newRule[10];
            for (int i = 0; i < tempRule->num_terms; i++)
            {
                if (tempRule->num_terms==1 && tempRule->terms[0] == val)
                {
                    removeRule(temp, val);
                    if (!temp->isResolved)
                    {
                        string epString[] = {"^"};
                        addRule(temp, epString, 1);
                    }
                }else if (tempRule->terms[i] == val)
                {
                    flag = true;
                }
                if(flag){
                    newRule[i] = tempRule->terms[i+1];
                }
                else{
                    
                }
            }
            if (flag)
            {
                addRule(temp, newRule, tempRule->num_terms);
                flag = false;
            }
            tempRule = tempRule->next;
        }
        temp->isResolved = true;
        temp = temp->prev;
    }
}

void resolveNull(Prod *tail)
{

    Prod *temp = tail;
    while (temp != nullptr)
    {

        if (temp->has_Epsilon)
        {
            cout << "has epsilon" << endl;
            removeRule(temp, "^");
            temp->has_Epsilon = false;
            removeNullTerm(tail, temp->NT);
        }

        temp = temp->prev;
    }
}

int main()
{
    Prod *prodHead = nullptr;
    Prod *prodTail = nullptr;
    string input;

    cout << "Enter production rules (e.g., S->a B), type 'exit' to stop:\n";

    // Loop to keep accepting user input
    while (true)
    {
        getline(cin, input);

        if (input == "exit")
        {
            break; // Stop the loop if the user types 'exit'
        }

        string prodNT, rule;
        // Split the input string
        splitString(input, "->", prodNT, rule);

        // Add the production
        Prod *prodAddress = addProd(&prodHead, prodNT);
        prodTail = prodAddress;

        // Use strtok to split the rule by '|'
        char ruleCStr[rule.length() + 1];
        strcpy(ruleCStr, rule.c_str()); // Convert string to C-style string

        char *subRule = strtok(ruleCStr, "|");
        while (subRule != nullptr)
        {
            string terms[10];
            int num_terms;
            splitRuleIntoTerms(subRule, terms, num_terms);

            // Add the sub-rule to the production
            addRule(prodAddress, terms, num_terms);

            // Move to the next sub-rule
            subRule = strtok(nullptr, "|");
        }
    }

    resolveNull(prodTail);
    // Print the productions and their rules
    cout << "\nProductions:\n";
    printProductions(prodHead);

    return 0;
}
