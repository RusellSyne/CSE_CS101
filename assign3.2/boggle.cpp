/**
 * File: boggle.cpp
 * ----------------
 * Find all words on the given spelling board and calculate the maximum possible score.
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "backtracking.h"
#include "gridlocation.h"
#include "grid.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "lexicon.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * Score words according to their length
 */
int points(string str) {
    if(str.size()<4)
        return 0;
    else
        return str.size()-3;
}

/*
 * All possible 8 moves for a location
 */
Queue<GridLocation> possibleMoves(GridLocation curLocation) {
    Queue<GridLocation> possibleNeighbors;
    int row =curLocation.row;
    int col =curLocation.col;
    possibleNeighbors.enqueue({row-1, col-1});
    possibleNeighbors.enqueue({row-1, col});
    possibleNeighbors.enqueue({row-1, col+1});
    possibleNeighbors.enqueue({row, col-1});
    possibleNeighbors.enqueue({row, col+1});
    possibleNeighbors.enqueue({row+1, col-1});
    possibleNeighbors.enqueue({row+1, col});
    possibleNeighbors.enqueue({row+1, col+1});
    return possibleNeighbors;
}

STUDENT_TEST("possible moves for a location") {
    GridLocation location = {1,1};
    Queue<GridLocation> possibleNeighbors{{0,0},{0,1},{0,2},{1,0},{1,2},{2,0},{2,1},{2,2}};
    EXPECT_EQUAL(possibleMoves(location), possibleNeighbors);
}
/*
 * Valid moves for a location
 */
Queue<GridLocation> generateValidMoves(Grid<char>& board, GridLocation curLocation) {
    Queue<GridLocation> neighbors;
    if(!board.inBounds(curLocation))
        error("The GridLocation is out of the Grid!");
    Queue<GridLocation> possibleNeighbors = possibleMoves(curLocation);
    while(!possibleNeighbors.isEmpty())
    {
        if (board.inBounds(possibleNeighbors.peek()))
            neighbors.enqueue(possibleNeighbors.dequeue());
        else
            possibleNeighbors.dequeue();
    }
    return neighbors;
}

STUDENT_TEST("Valid moves for a location") {
    GridLocation location = {0,1};
    GridLocation location2 = {0,3};
    GridLocation location3 = {1,0};
    Grid<char> board = {{'B','C','D','F'},
                        {'G','H','J','K'},
                        {'L','M','N','P'},
                        {'Q','R','S','T'}};
    Queue<GridLocation> neighbors{{0,0},{0,2},{1,0},{1,1},{1,2}};
    Queue<GridLocation> neighbors2{{0,2},{1,2},{1,3}};
    Queue<GridLocation> neighbors3{{0,0},{0,1},{1,1},{2,0},{2,1}};
    cout<<endl<<board.get({0,1})<<endl;
    cout<<endl<<board.get({2,1})<<endl;
    EXPECT_EQUAL(generateValidMoves(board,location), neighbors);
    EXPECT_EQUAL(generateValidMoves(board,location2), neighbors2);
    EXPECT_EQUAL(generateValidMoves(board,location3), neighbors3);
}

/*
 * Find all words on the given spelling board from one given initial position.
 */
int scoreBoardBranch(Grid<char>& board, Lexicon& lex,  Set<string> &wordRecord, GridLocation curLocation, Set<GridLocation> locPassed, string curWord) {

    if(!lex.containsPrefix(curWord))
        return 0;

    Queue<GridLocation> neighbors = generateValidMoves(board, curLocation);
    int result = 0;
    if(lex.contains(curWord) && curWord.size()>3 && !wordRecord.contains(curWord))  {
        wordRecord.add(curWord);
        result += points(curWord);
    }

    while(!neighbors.isEmpty()) {
        if(locPassed.contains(neighbors.peek()))
            neighbors.dequeue();
        else {
            Set<GridLocation> locPassedNext = locPassed;
            locPassedNext.add(neighbors.peek());
            string nextWord = curWord;
            nextWord.push_back(board.get(neighbors.peek()));
            result += scoreBoardBranch(board,lex,wordRecord,neighbors.dequeue(),locPassedNext, nextWord);
        }
    }
    return result;
}

/*
 * Find all words on the given spelling board and calculate the maximum possible score.
 */
int scoreBoard(Grid<char>& board, Lexicon& lex) {
    int totalScore = 0;
    Set<string> wordRecord;
    for(int i=0;i<board.numCols();i++)
    {
        for(int j=0;j<board.numRows();j++)
        {
            GridLocation iniLocation={i, j};
            Set<GridLocation> locPassed = {iniLocation};
            string curWord;
            curWord.push_back(board.get(iniLocation));
            totalScore += scoreBoardBranch(board,lex,wordRecord,iniLocation,locPassed, curWord);
        }
    }
    return totalScore;
}

/* * * * * * Test Cases * * * * * */

/* Test helper function to return shared copy of Lexicon. Use to
 * avoid (expensive) re-load of word list on each test case. */
static Lexicon& sharedLexicon() {
    static Lexicon lex("res/EnglishWords.txt");
    return lex;
}

PROVIDED_TEST("Load shared Lexicon, confirm number of words") {
    Lexicon lex = sharedLexicon();
    EXPECT_EQUAL(lex.size(), 127145);
}

PROVIDED_TEST("Test point scoring") {
    EXPECT_EQUAL(points("and"), 0);
    EXPECT_EQUAL(points("quad"), 1);
    EXPECT_EQUAL(points("quint"), 2);
    EXPECT_EQUAL(points("sextet"), 3);
    EXPECT_EQUAL(points("seventh"), 4);
    EXPECT_EQUAL(points("supercomputer"), 10);
}

PROVIDED_TEST("Test scoreBoard, board contains no words, score of zero") {
    Grid<char> board = {{'B','C','D','F'}, //no vowels, no words
                        {'G','H','J','K'},
                        {'L','M','N','P'},
                        {'Q','R','S','T'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 0);
}


PROVIDED_TEST("Test scoreBoard, alternate paths for same word, still score of 1") {
    Grid<char> board = {{'C','C','_','_'},
                        {'C','Z','C','_'},
                        {'_','A','_','_'},
                        {'R','_','R','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

PROVIDED_TEST("Test scoreBoard, small number of words in corner of board") {
    Grid<char> board = {{'L','I','_','_'},
                        {'M','E','_','_'},
                        {'_','S','_','_'},
                        {'_','_','_','_'}};
    Set<string> words = {"SMILE", "LIMES", "MILES", "MILE", "MIES", "LIME", "LIES", "ELMS", "SEMI"};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()),  2 + 2 + 2 + 1 + 1 + 1 + 1 + 1 + 1);
}

PROVIDED_TEST("Test scoreBoard, full board, small number of words") {
    Grid<char> board = {{'E','Z','R','R'},
                        {'O','H','I','O'},
                        {'N','J','I','H'},
                        {'Y','A','H','O'}};
    Set<string> words = { "HORIZON", "OHIA", "ORZO", "JOHN", "HAJI"};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 4 + 1 + 1 + 1 + 1);
}

PROVIDED_TEST("Test scoreBoard, full board, medium number of words") {
    Grid<char> board = {{'O','T','H','X'},
                        {'T','H','T','P'},
                        {'S','S','F','E'},
                        {'N','A','L','T'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 76);
}

PROVIDED_TEST("Test scoreBoard, full board, large number of words") {
    Grid<char> board = {{'E','A','A','R'},
                        {'L','V','T','S'},
                        {'R','A','A','N'},
                        {'O','I','S','E'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 234);
}
