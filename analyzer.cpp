#include "analyzer.h"

// Scans individual explicitly defined IDENTIFIER tokens rather than raw strings to prevent substring false-positives
vector<string> findUnused(const vector<Token>& tokens, vector<Symbol> table)
{
    vector<string> unused;

    for(auto s : table)
    {
        int count = 0;
        
        for(auto t : tokens) {
            if (t.type == "IDENTIFIER" && t.value == s.name) {
                count++;
            }
        }

        // If the variable only appears once in the entire token stream, it is only declared but never actually used.
        if (count <= 1)
            unused.push_back(s.name);
    }

    return unused;
}

// Scans only functional IDENTIFIERS, preventing false positives inside comments, variables, or strings
vector<string> securityCheck(const vector<Token>& tokens)
{
    vector<string> issues;

    for(auto t : tokens) {
        if (t.type == "IDENTIFIER") {
            if (t.value == "gets")
                issues.push_back("Unsafe function: gets()");
            if (t.value == "strcpy")
                issues.push_back("Unsafe function: strcpy()");
        }
    }

    return issues;
}

// Computes graph complexity pathing precisely based on control flow KEYWORDS and Logicals
int cyclomaticComplexity(const vector<Token>& tokens)
{
    int complexity = 1;

    for(auto t : tokens) {
        if (t.type == "KEYWORD") {
            if (t.value == "if" || t.value == "for" || t.value == "while") {
                complexity++;
            }
        } else if (t.type == "OPERATOR") {
            if (t.value == "&&" || t.value == "||") {
                complexity++;
            }
        }
    }

    return complexity;
}