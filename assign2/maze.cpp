/*
* File: maze.cpp
* ----------------------
* This cpp file can give a solution for a maze
*/
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;


/* Find all the valid moves for the given position in a maze.
 */
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    /* TODO: Fill in the remainder of this function. */
    if ((cur.row >= maze.numRows()) || (cur.col >= maze.numCols()) || (cur.row < 0) || (cur.col < 0))
    {
        error("Error!!! The GridLocation is out of the Grid!");
    }
    if (cur.row > 0)
    {
        if (maze[cur.row - 1][cur.col])
            neighbors.add({cur.row - 1,cur.col});
    }
    if (cur.col > 0)
    {
        if (maze[cur.row][cur.col - 1])
            neighbors.add({cur.row,cur.col - 1});

    }
    if (cur.row < maze.numRows()-1)
    {
        if ( maze[cur.row + 1][cur.col])
            neighbors.add({cur.row + 1,cur.col});

    }
    if ((cur.col < maze.numCols()-1))
    {
        if ( maze[cur.row][cur.col+1])
            neighbors.add({cur.row,cur.col + 1});

    }
    return neighbors;
}

/* Validate a solution for a maze
 */
void validatePath(Grid<bool>& maze, Stack<GridLocation> path) {

    if (!maze[0][0] || !maze[maze.numRows()-1][maze.numCols()-1])
    {
        error("The maze enrty or exit is not true!");
    }

    GridLocation mazeExit = {maze.numRows()-1,  maze.numCols()-1};
    if (path.peek() != mazeExit) {
        error("Path does not end at maze exit");
    }

    Set<GridLocation> pathCollection;
    int pathSize = path.size();
    for (int i=0;i<pathSize -1;i++)
    {
        GridLocation curlocation=path.pop();
        GridLocation nextLocation=path.peek();
        pathCollection.add(curlocation);
        Set<GridLocation> neighbors = generateValidMoves( maze, curlocation);
        if (!neighbors.contains(nextLocation))
        {
            error("The path goes through the wall or been teleported! The wrong location is {" + to_string(nextLocation.row) + "," + to_string(nextLocation.col) + "}");
        }
        if (pathCollection.contains(nextLocation))
        {
             error("The path of the maze contains a duplicate position! The wrong location is {" + to_string(nextLocation.row) + "," + to_string(nextLocation.col) + "}");
        }
    }
    GridLocation mazeEntry = {0, 0};
    if (path.peek() != mazeEntry) {
        error("Path does not begin at maze entry");
    }
}

/* Convert the datatype Stack<GridLocation> to Set<GridLocation>
 */
Set<GridLocation> stackToSet(Stack<GridLocation> stackData)
{
    Set<GridLocation> setData;
    while (!stackData.isEmpty())
    {
        setData.add(stackData.pop());
    }
    return setData;
}

/* Find a solution for a maze
 */
Stack<GridLocation> solveMaze(Grid<bool>& maze) {
    MazeGraphics::drawGrid(maze);
    Stack<GridLocation> iniPath = {{0,0}};
    /* TODO: Fill in the remainder of this function. */
    Queue<Stack<GridLocation>> pathRecord;
    pathRecord.enqueue(iniPath);

    GridLocation mazeExit = {maze.numRows()-1,  maze.numCols()-1};

    while (!pathRecord.isEmpty())
    {
        Stack<GridLocation> pathCur = pathRecord.dequeue();
        if (pathCur.peek()==mazeExit)
        {
            MazeGraphics::highlightPath(pathCur, "red", 5);
            return pathCur;
        }
        //collect the locations that have been passed
        Set<GridLocation> locationPassed = stackToSet(pathCur);
        Set<GridLocation> neighbors = generateValidMoves(maze, pathCur.peek());
        while (!neighbors.isEmpty())
        {
            GridLocation neighborsFirst = neighbors.first();
            neighbors.remove(neighborsFirst);
            if(!locationPassed.contains(neighborsFirst))
            {
                Stack<GridLocation> *pathNew = new Stack<GridLocation>(pathCur);
                pathNew->push(neighborsFirst);
                pathRecord.enqueue(*pathNew);
                //MazeGraphics::highlightPath(*pathNew, "red", 1);
                delete pathNew;
                pathNew = NULL;
            }

        }

    }
    error("No solution for this maze!");
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Stack<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0,0}, {0,2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}



PROVIDED_TEST("validatePath on correct solution") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for medium maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for large maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };
    Stack<GridLocation> revisit = { {0 ,0}, {1, 0}, {0, 0}, {1, 0}, {1, 1} };

    EXPECT_ERROR(validatePath(maze, not_end_at_exit));
    EXPECT_ERROR(validatePath(maze, not_begin_at_entry));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
    EXPECT_ERROR(validatePath(maze, teleport));
    EXPECT_ERROR(validatePath(maze, revisit));
}



PROVIDED_TEST("solveMaze on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMaze on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

// TODO: add your test cases here
STUDENT_TEST("stackToSet") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Stack<GridLocation> stackData = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(stackToSet(stackData), expected);
}

STUDENT_TEST("generateValidMoves on corner of 2x2 grid with all walls") {
    Grid<bool> maze = {{false, false},
                       {false, false}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

STUDENT_TEST("test boundary condition - 1") {
    Grid<bool> maze = {{false, true},
                       {false, false}};
    GridLocation corner = {1, 1};
    Set<GridLocation> expected = {{0, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

STUDENT_TEST("test boundary condition - 2") {
    Grid<bool> maze = {{false, true},
                       {false, false}};
    GridLocation corner = {-1, 1};
    Set<GridLocation> expected = {};

    EXPECT_ERROR(generateValidMoves(maze, corner));
}

STUDENT_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze1 = {{true, false},
                       {true, false}};
    Grid<bool> maze2 = {{false, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };

    EXPECT_ERROR(validatePath(maze1, not_end_at_exit));
    EXPECT_ERROR(validatePath(maze2, not_begin_at_entry));

}
