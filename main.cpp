#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "lexer.h"
#include "parser.h"
#include "symbol_table.h"
#include "analyzer.h"
#include "tac.h"
#include "ml_corrector.h"

using namespace std;

void printBreak() {
    cout << string(60, '-') << "\n";
}

int main()
{
    ifstream file("test_code/sample.c");
    stringstream buffer;
    buffer << file.rdbuf();
    string code = buffer.str();

    auto tokens = tokenize(code);
    
    // ----- ML TRAINING PHASE ----- //
    string trainingData = "int main ( ) { int a = 5 ; float b = 3.14 ; if ( a > 0 ) { return a ; } else { return 0 ; } int x ; }";
    auto trainingTokens = tokenize(trainingData);
    MLModel ml;
    ml.train(trainingTokens); // The model learns probabilities

    cout << "\n"; printBreak();
    cout << "  MACHINE LEARNING ERROR CORRECTION\n"; printBreak();
    auto corrections = ml.performMLCorrection(tokens); 
    if(corrections.empty()) {
        cout << "| No typos detected (Probability 100% matched)           |\n";
    } else {
        cout << left << "| " << setw(8) << "LINE" << "| " << setw(15) << "ORIGINAL" << "| " << setw(15) << "CORRECTED" << "| " << "CONFIDENCE |\n";
        printBreak();
        for(auto c : corrections) {
            cout << left << "| " << setw(8) << c.line << "| " << setw(15) << c.original << "| " << setw(15) << c.corrected 
                 << "| " << int(c.probability * 100) << "%       |\n";
        }
    }

    printBreak();
    cout << "\n"; printBreak();
    cout << "  LEXICAL ANALYSIS TOKENS\n"; printBreak();
    cout << left << "| " << setw(20) << "TOKEN TYPE" << "| " << setw(25) << "VALUE" << "| " << "LINE |\n";
    printBreak();
    for(auto t : tokens) {
         cout << left << "| " << setw(20) << t.type << "| " << setw(25) << t.value << "| " << t.line << "    |\n";
    }
    printBreak();

    cout << "\n"; printBreak();
    cout << "  ABSTRACT SYNTAX TREE (AST)\n"; printBreak();
    ASTNode* root = parse(tokens);
    string astOutput = "";
    printAST(root, 0, astOutput);
    cout << astOutput;
    printBreak();

    if (parserError != "") {
        cout << "\n"; printBreak();
        cout << "  SYNTAX ERROR\n"; printBreak();
        cout << "Syntax Error (Line " << errorLine << "): " << parserError << "\n";
        cout << "|||ERROR_LINE:" << errorLine << "|||\n"; // Signals UI to highlight
        printBreak();
    } else {
        cout << "\n"; printBreak();
        cout << "  SYNTAX CHECK\n"; printBreak();
        cout << "Syntax OK\n";
        printBreak();
        
        cout << "\n"; printBreak();
        cout << "  THREE ADDRESS CODE (TAC)\n"; printBreak();
        auto tac = generateTAC(root);
        if(tac.empty()) cout << "(No TAC generated for flat structure)" << endl;
        for(auto t : tac) cout << "  " << t << endl;
        printBreak();
    }

    cout << "\n"; printBreak();
    cout << "  SYMBOL TABLE\n"; printBreak();
    auto table = buildSymbolTable(code);
    if(table.empty()) {
        cout << "| (Empty)                                                  |\n";
    } else {
        cout << left << "| " << setw(20) << "DATA TYPE" << "| " << "VARIABLE NAME                |\n";
        printBreak();
        for(auto s : table) {
             cout << left << "| " << setw(20) << s.type << "| " << setw(29) << s.name << "|\n";
        }
    }
    printBreak();

    cout << "\n"; printBreak();
    cout << "  STATIC CODE ANALYSIS\n"; printBreak();
    auto unused = findUnused(tokens,table);
    for(auto u : unused) cout << "[WARNING] Unused Variable: " << u << endl;

    auto issues = securityCheck(tokens);
    for(auto i : issues) cout << "[SECURITY] " << i << endl;

    cout << "[METRIC] Cyclomatic Complexity: " << cyclomaticComplexity(tokens) << endl;
    printBreak();
    
    return 0;
}