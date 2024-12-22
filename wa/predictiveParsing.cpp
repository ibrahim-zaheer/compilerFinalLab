#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>

using namespace std;

// Enumerations for non-terminals and terminals
enum NonTerminals
{
    FUNCTION,
    ARGLIST,
    ARGLIST_PRIME,
    ARG,
    STMT,
    DECLARATION,
    TYPE,
    WHILESTMT,
    IFSTMT,
    ELSEPART,
    COMPOUNDSTMT,
    STMTLIST,
    STMTLIST_PRIME,
    BOOLEXPR,
    COMPARE,
    // A,
    // B,
    EXPR,
    MAG,
    MAG_PRIME,
    TERM,
    TERM_PRIME,
    FACTOR,
    NON_TERMINAL_COUNT
};
enum Terminals
{
    IDENTIFIER,
    OPEN_PAREN,
    CLOSE_PAREN,
    COMMA,
    SEMICOLON,
    INT,
    FLOAT,
    WHILE,
    IF,
    ELSE,
    OPEN_BRACE,
    CLOSE_BRACE,
    EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,
    NOT_EQUAL,
    PLUS,
    MINUS,
    MUL,
    DIV,
    NUMBER,
    END_MARKER,
    TERMINAL_COUNT
};

string arr[NON_TERMINAL_COUNT][TERMINAL_COUNT] = {};

void initializeArray()
{
    for (int i = 0; i < NON_TERMINAL_COUNT; i++)
    {
        for (int j = 0; j < TERMINAL_COUNT; j++)
        {
            arr[i][j] = "-";
        }
    }

    arr[FUNCTION][INT] = "Type identifier ( ArgList ) CompoundStmt";
    arr[FUNCTION][FLOAT] = "Type identifier ( ArgList ) CompoundStmt";

    arr[ARGLIST][INT] = "Arg ARGLIST_PRIME";
    arr[ARGLIST][FLOAT] = "Arg ARGLIST_PRIME";
    arr[ARGLIST_PRIME][COMMA] = ", Arg ARGLIST_PRIME";
    arr[ARGLIST_PRIME][CLOSE_PAREN] = "^";

    arr[ARG][INT] = "Type identifier";
    arr[ARG][FLOAT] = "Type identifier";

    arr[STMT][IDENTIFIER] = "Expr ;";
    arr[STMT][INT] = "Declaration";
    arr[STMT][FLOAT] = "Declaration";
    arr[STMT][WHILE] = "WhileStmt";
    arr[STMT][IF] = "IfStmt";
    arr[STMT][OPEN_PAREN] = "CompoundStmt";

    arr[DECLARATION][INT] = "Type identifier";
    arr[DECLARATION][FLOAT] = "Type identifier";

    arr[TYPE][INT] = "int";
    arr[TYPE][FLOAT] = "float";

    arr[WHILESTMT][WHILE] = "while (BoolExpr) Stmt";

    arr[IFSTMT][IF] = "if (BoolExpr) Stmt ElsePart";

    arr[ELSEPART][IDENTIFIER] = "^";
    arr[ELSEPART][INT] = "^";
    arr[ELSEPART][FLOAT] = "^";
    arr[ELSEPART][WHILE] = "^";
    arr[ELSEPART][IF] = "^";
    arr[ELSEPART][OPEN_PAREN] = "^";
    arr[ELSEPART][ELSE] = "else Stmt";

    arr[COMPOUNDSTMT][OPEN_BRACE] = "{ StmtList }";

    arr[STMTLIST][IDENTIFIER] = "STMTLIST_PRIME";
    arr[STMTLIST][INT] = "STMTLIST_PRIME";
    arr[STMTLIST][FLOAT] = "STMTLIST_PRIME";
    arr[STMTLIST][WHILE] = "STMTLIST_PRIME";
    arr[STMTLIST][IF] = "STMTLIST_PRIME";
    arr[STMTLIST][OPEN_PAREN] = "STMTLIST_PRIME";
    arr[STMTLIST][CLOSE_PAREN] = "^";

    arr[STMTLIST_PRIME][IDENTIFIER] = "STMT STMTLIST_PRIME";
    arr[STMTLIST_PRIME][INT] = "STMT STMTLIST_PRIME";
    arr[STMTLIST_PRIME][FLOAT] = "STMT STMTLIST_PRIME";
    arr[STMTLIST_PRIME][WHILE] = "STMT STMTLIST_PRIME";
    arr[STMTLIST_PRIME][IF] = "STMT STMTLIST_PRIME";
    arr[STMTLIST_PRIME][OPEN_PAREN] = "STMT STMTLIST_PRIME";
    arr[STMTLIST_PRIME][CLOSE_PAREN] = "^";

    arr[BOOLEXPR][IDENTIFIER] = "identifier Compare Mag";

    arr[COMPARE][EQUAL] = "= =";
    arr[COMPARE][LESS_THAN] = "< A";
    arr[COMPARE][GREATER_THAN] = "> B";

    // arr[A][IDENTIFIER] = "^";
    // arr[A][OPEN_PAREN] = "^";
    // arr[A][NUMBER] = "^";
    // arr[A][EQUAL] = "=";
    // arr[A][GREATER_THAN] = ">";

    // arr[B][IDENTIFIER] = "^";
    // arr[B][OPEN_PAREN] = "^";
    // arr[B][NUMBER] = "^";
    // arr[B][EQUAL] = "=";

    arr[EXPR][IDENTIFIER] = "identifier = Mag";

    arr[MAG][OPEN_PAREN] = "Term MAG_PRIME";
    arr[MAG][IDENTIFIER] = "Term MAG_PRIME";
    arr[MAG][NUMBER] = "Term MAG_PRIME";

    arr[MAG_PRIME][CLOSE_PAREN] = "^";
    arr[MAG_PRIME][SEMICOLON] = "^";
    arr[MAG_PRIME][PLUS] = "Term MAG_PRIME";
    arr[MAG_PRIME][MINUS] = "Term MAG_PRIME";

    arr[TERM][OPEN_PAREN] = "^";
    arr[TERM][SEMICOLON] = "^";
    arr[TERM][MUL] = "Factor TERM_PRIME";
    arr[TERM][DIV] = "Factor TERM_PRIME";

    arr[TERM_PRIME][OPEN_PAREN] = "^";
    arr[TERM_PRIME][SEMICOLON] = "^";
    arr[TERM_PRIME][MUL] = "Factor TERM_PRIME";
    arr[TERM_PRIME][DIV] = "Factor TERM_PRIME";

    arr[FACTOR][IDENTIFIER] = "identifier";
    arr[FACTOR][OPEN_PAREN] = "( Expr )";
    arr[FACTOR][NUMBER] = "NUMBER";
}

unordered_map<string, NonTerminals> stringToEnum = {
    {"FUNCTION", FUNCTION},
    {"ARGLIST", ARGLIST},
    {"ARGLIST_PRIME", ARGLIST_PRIME},
    {"ARG", ARG},
    {"STMT", STMT},
    {"DECLARATION", DECLARATION},
    {"TYPE", TYPE},
    {"WHILESTMT", WHILESTMT},
    {"IFSTMT", IFSTMT},
    {"ELSEPART", ELSEPART},
    {"COMPOUNDSTMT", COMPOUNDSTMT},
    {"STMTLIST", STMTLIST},
    {"STMTLIST_PRIME", STMTLIST_PRIME},
    {"BOOLEXPR", BOOLEXPR},
    {"COMPARE", COMPARE},
    {"EXPR", EXPR},
    {"MAG", MAG},
    {"MAG_PRIME", MAG_PRIME},
    {"TERM", TERM},
    {"TERM_PRIME", TERM_PRIME},
    {"FACTOR", FACTOR}};

string toLowerCase(const string &str)
{
    string result = str; // Create a copy of the input string
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

string toUpperCase(const string &str)
{
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool isNumber(const string &str)
{
    // Regex to match numbers (integers or floats)
    regex numberPattern("^[+-]?([0-9]+(\\.[0-9]*)?|\\.[0-9]+)$");
    return regex_match(str, numberPattern);
}

int getType(string str)
{

    if (str.length() == 1 && !isupper(str[0]))
    {
        char c = str[0];
        if (isalpha(c))
        {
            return IDENTIFIER;
        }

        else if (c == '(')
        {
            return OPEN_PAREN;
        }
        else if (c == ')')
        {
            return CLOSE_PAREN;
        }
        else if (c == '{')
        {
            return OPEN_BRACE;
        }
        else if (c == '}')
        {
            return CLOSE_BRACE;
        }
        else if (c == ';')
        {
            return SEMICOLON;
        }
        else if (c == '+')
        {
            return PLUS;
        }
        else if (c == '-')
        {
            return MINUS;
        }
        else if (c == '*')
        {
            return MUL;
        }
        else if (c == '/')
        {
            return DIV;
        }
        else if (c == ',')
        {
            return COMMA;
        }
        else if (c == '<')
        {
            return LESS_THAN;
        }
        else if (c == '>')
        {
            return GREATER_THAN;
        }

        return -1; // Or a custom INVALID/UNKNOWN type
    }
    else
    {
        if (isNumber(str))
        {
            return NUMBER;
        }
        else if (toLowerCase(str) == "else")
        {
            return ELSE;
        }
        else if (toLowerCase(str) == "if")
        {
            return IF;
        }
        else if (toLowerCase(str) == "while")
        {
            return WHILE;
        }
        else if (toLowerCase(str) == "int")
        {
            return INT;
        }
        else if (toLowerCase(str) == "float")
        {
            return FLOAT;
        }
        else if (str == "==")
        {
            return EQUAL;
        }
        else if (str == "<=")
        {
            return LESS_THAN_EQUAL;
        }
        else if (str == ">=")
        {
            return GREATER_THAN_EQUAL;
        }
        else if (str == "<>")
        {
            return NOT_EQUAL;
        }
        else if (str == "IDENTIFIER" || (isalpha(str[0]) && all_of(str.begin(), str.end(), ::islower)))
        {
            return IDENTIFIER;
        }
        return -1; // Or a custom INVALID/UNKNOWN type
    }
}

vector<string> splitBySpaces(const string &input)
{
    vector<string> tokens;
    istringstream stream(input);
    string token;

    while (stream >> token)
    {
        tokens.push_back(token);
    }

    return tokens;
}

void parseString(string &inputString)
{
    stack<string> parsingStack;
    parsingStack.push("$");        // End marker
    parsingStack.push("FUNCTION"); // Start symbol
    inputString += " $";
    int stringIndex = 0;

    vector<string> tokenizedString = splitBySpaces(inputString);

    while (!parsingStack.empty())
    {

        if (stringIndex == tokenizedString.size())
        {
            cout << "Error parsing string" << endl;
            return;
        }
        string stackValue = toUpperCase(parsingStack.top());
        parsingStack.pop();

        string currentStringValue = tokenizedString[stringIndex];

        cout << "Stack value: " << stackValue << endl;

        if (toUpperCase(currentStringValue) == stackValue || getType(currentStringValue) == getType(stackValue))
        {
            cout << "Matched: " << currentStringValue << endl;
            stringIndex++;
        }
        else if (stackValue == "$")
        {
            if (currentStringValue == stackValue)
            {
                cout << "Processing ended, $ matched" << endl;
            }
            else
            {
                cout << "Stack pushed $ but string ended";
            }
        }
        else
        {
            if (stringToEnum.find(stackValue) != stringToEnum.end())
            {
                NonTerminals rowNumber = stringToEnum[stackValue];
                string newString = arr[rowNumber][getType(currentStringValue)];

                if (newString != "-" && newString != "^")
                {
                    vector<string> tokens = splitBySpaces(newString);

                    for (int i = tokens.size() - 1; i >= 0; --i)
                    {
                        parsingStack.push(tokens[i]);
                    }
                }
            }
            else
            {
                cout << "Stack value: " << stackValue << " NOT FOUND" << endl;
            }
        }
    }
}

int main()
{
    string input;

    cout << "Enter production rules (e.g., S->a B):\n";

    getline(cin, input);

    initializeArray();
    parseString(input);
}