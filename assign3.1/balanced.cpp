/**
 * File: balanced.cpp
 * ----------------
 * Determines whether the parentheses in an expression match correctly.
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "recursion.h"
#include "testing/SimpleTest.h"

using namespace std;

/*
 * Receives a string parameter and returns a string containing only parenthesis operators
 */
string operatorsFrom(string str) {
    if (str.empty()) {
        return "";
    }
    if (startsWith(str, "[") || startsWith(str, "]") || startsWith(str, "(") || startsWith(str, ")") || startsWith(str, "{") || startsWith(str, "}")) {
         return str[0] + operatorsFrom(str.substr(1));
    }
    else {
        return operatorsFrom(str.substr(1));
    }
}

/*
 * Determines whether parentheses in an expression match correctly
 */
bool operatorsAreMatched(string ops) {
    if (ops=="") {
        return true;
    }
    else if(stringContains(ops,"()"))
    {
        return operatorsAreMatched(stringReplace(ops,"()",""));
    }
    else if(stringContains(ops,"[]"))
    {
        return operatorsAreMatched(stringReplace(ops,"[]",""));
    }
    else if(stringContains(ops,"{}"))
    {
        return operatorsAreMatched(stringReplace(ops,"{}",""));
    }
    return false;
}

/*
 * The isBalanced function assumes correct implementation of
 * the above two functions operatorsFrom and operatorsMatch.
 * It uses operatorsFrom to extract the operator characters
 * from the input string and then confirms that those
 * operators are balanced by using operatorsMatch.
 * You should not modify the provided code in the isBalanced
 * function.  If the previous two functions have been implemented
 * correctly, the provided isBalanced will correctly report whether
 * the input string has balanced bracketing operators.
 */
bool isBalanced(string str) {
    string ops = operatorsFrom(str);
    return operatorsAreMatched(ops);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("operatorsFrom on simple example") {
    EXPECT_EQUAL(operatorsFrom("vec[3]"), "[]");
    EXPECT_EQUAL(operatorsFrom("vec[3](387)[12345]"), "[]()[]");
    EXPECT_EQUAL(operatorsFrom("__[(12345abc)234]"), "[()]");
}

PROVIDED_TEST("operatorsAreMatched on simple example") {
    EXPECT(operatorsAreMatched("{}"));
    EXPECT(operatorsAreMatched("{([])}"));
    EXPECT(operatorsAreMatched("({})"));
    EXPECT(!operatorsAreMatched("{)}"));
}

PROVIDED_TEST("isBalanced on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT(isBalanced(example));
}

PROVIDED_TEST("isBalanced on non-balanced examples from writeup") {
    EXPECT(!isBalanced("( ( [ a ] )"));
    EXPECT(!isBalanced("3 ) ("));
    EXPECT(!isBalanced("{ ( x } y )"));
}
