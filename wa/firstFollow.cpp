#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

struct rule{
    string arg;
    rule *next = NULL;
    rule *prev = NULL;
};

struct production{
    string title;
    production *next = NULL;
    production *prev = NULL;
    rule *rule_head = NULL;
};

production *header;
production *tail;
map<string, set<string>> First_Set;
map<string, set<string>> Follow_Set;

void add_production(){
    production *temp_header = header;
    int cont_flag = 0;
    while (1){
        cout << "\nAdd a Production? (-1 to Exit/0 to Continue): ";
        cin >> cont_flag;
        if(cont_flag == -1){
            temp_header -> prev -> next = NULL;
            break;
        }
        cin.ignore();
        cout << "Enter Production Title: ";
        getline(cin, temp_header -> title);
        temp_header -> rule_head = new rule();
        rule *temp_rule_head = temp_header -> rule_head;
        while(1){
            string rule_arg;
            cout << "Enter a Rule(-1 to Exit): ";
            getline(cin, rule_arg);
            if (rule_arg == "-1") {
                temp_rule_head -> prev -> next = NULL;
                break;
            }
            temp_rule_head -> arg = rule_arg;
            temp_rule_head -> next = new rule();
            temp_rule_head -> next -> prev = temp_rule_head;
            temp_rule_head = temp_rule_head -> next;
        }
        temp_header -> next = new production();
        temp_header -> next -> prev = temp_header;
        temp_header = temp_header -> next;
    }
    tail = temp_header -> prev;
}

void print_productions(){
    production *temp_header = header;
    cout << "\nGrammer:- \n";
    while(temp_header != NULL){
        cout << temp_header -> title;
        cout << " -> ";
        rule *temp_rule_head = temp_header -> rule_head;
        while(temp_rule_head != NULL){
            cout << temp_rule_head -> arg;
            cout << " | ";
            temp_rule_head = temp_rule_head -> next;
        }
        cout << "\n";
        temp_header = temp_header -> next;
    }
}

/*
1. First(Terminal) = Terminal - Like a in aA
2. First(Null/#) = Null/#
3. First(Non-Terminal) = First(First Character of Non-Terminal)
*/
void first_set(){
    production *temp_tail = tail;
    while(temp_tail != NULL){
        rule *temp_rule_head = temp_tail -> rule_head;
        while(temp_rule_head != NULL){
            string arg = temp_rule_head -> arg;
            if(arg == "#" || arg == "id"){
                First_Set[temp_tail -> title].insert(arg);
            }
            else{
                production *temp_iterator = header;
                bool non_terminal_found = false;
                string first_term;
                first_term += arg[0];
                if(arg[1] == '\''){
                    first_term += '\'';
                }
                while(temp_iterator != NULL){
                    if(temp_iterator -> title == first_term){
                        non_terminal_found = true;
                        break;
                    }
                    temp_iterator = temp_iterator -> next;
                }
                if(non_terminal_found){
                    First_Set[temp_tail -> title].insert(First_Set[first_term].begin(), First_Set[first_term].end());
                }
                else{
                    First_Set[temp_tail -> title].insert(first_term);
                }
            }
            temp_rule_head = temp_rule_head -> next;
        }
        temp_tail = temp_tail -> prev;
    }

    cout << "\nFirst Sets:- \n";
    for (const auto& entry : First_Set) {
        cout << entry.first << " = { ";
        for (const string& terminal : entry.second) {
            cout << terminal << ", ";
        }
        cout << "}" << endl;
    }
}

/*
1. Starting Non-Terminal will always have a $.
2. Check instances of non-terminal in the full grammer.
3. If the second character is a terminal in the instance -> Follow of that instance would be that terminal.
4. If the second character is a non-terminal in the instance ->  First(Second Character). If there is #, we have to take follow(parent) instead of #.
5. If the second character is not present in the instance -> Follow of that instance would be Follow(Parent). If the parent is the same as the instance, we skip it in RRG.
*/
void follow_set(){
    production *temp_head = header;
    Follow_Set[temp_head -> title].insert("$");
    while(temp_head != NULL){               //Main Loop for picking the Non Terminals.
        string current_non_terminal = temp_head -> title;
        production *temp_head_iterator = header;
        while(temp_head_iterator != NULL){  //Sub Loop to search through the Grammer for instances of the current non terminal, Production wise.
            rule *temp_rule_head = temp_head_iterator -> rule_head;
            while(temp_rule_head != NULL){  //Looking in individual rules.
                string arg = temp_rule_head -> arg;
                size_t pos = arg.find(current_non_terminal);
                if(arg.find(current_non_terminal) == string::npos){
                    temp_rule_head = temp_rule_head -> next;
                    continue;
                }
                else{
                    int second_char_pos = pos + current_non_terminal.length();
                    if(arg[second_char_pos] == '\''){  //So, we don't pick up E' when doing E.
                        temp_rule_head = temp_rule_head -> next;
                        continue;
                    }
                    else{
                        if(arg.length() == second_char_pos){ //No Character afterwards/5.
                            if(current_non_terminal != temp_head_iterator -> title){
                                Follow_Set[current_non_terminal].insert(Follow_Set[temp_head_iterator -> title].begin(), Follow_Set[temp_head_iterator -> title].end());
                            }  
                        }
                        else{
                            //Need to check if the present chararcter is a Terminal or Not.
                            string second_term; 
                            second_term += arg[second_char_pos];
                            if(arg[second_char_pos + 1] == '\''){
                                second_term += '\'';
                            }
                            production *check_iterator = header;
                            bool non_terminal_found = false;
                            while(check_iterator != NULL){
                                if(check_iterator -> title == second_term){
                                    non_terminal_found = true;
                                    break;
                                }
                                check_iterator = check_iterator -> next;
                            }
                            if(non_terminal_found){ // Non Terminal/4.
                                for (const string& element : First_Set[second_term]) {
                                    if (element != "#") {
                                        Follow_Set[current_non_terminal].insert(element);
                                    } else {
                                        Follow_Set[current_non_terminal].insert(Follow_Set[temp_head_iterator -> title].begin(), Follow_Set[temp_head_iterator -> title].end());
                                    }
                                }                           
                            }
                            else{ // Terminal/3.
                                Follow_Set[current_non_terminal].insert(second_term);
                            }
                        }
                    }
                }
                temp_rule_head = temp_rule_head -> next;
            }
            temp_head_iterator = temp_head_iterator -> next;
        }
        temp_head = temp_head -> next;
    }
    
    cout << "\nFollow Sets:- \n";
    for (const auto& entry : Follow_Set) {
        cout << entry.first << " = { ";
        for (const string& terminal : entry.second) {
            cout << terminal << ", ";
        }
        cout << "}" << endl;
    }
}

int main(){
    header = new production();
    add_production();
    print_productions();
    first_set();
    follow_set();
}