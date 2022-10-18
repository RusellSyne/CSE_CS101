/**
 * File: sierpinski.cpp
 * ----------------
 *  Draw a self similar fractal triangle
 */
#include <iostream>    // for cout, endl
#include "recursion.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * This function draws a filled, black triangle on the provided GWindow
 * that is defined by the corner GPoints one, two, and three. This
 * function has been provided for you and does not need to be modified.
 */
void fillBlackTriangle(GWindow& window, GPoint one, GPoint two, GPoint three) {
    window.setColor("black");
    window.fillPolygon( {one, two, three} );
}


/*
 * Overloaded operator + for GPoint
 */
GPoint operator+(GPoint &point1, GPoint &point2) {
    GPoint pointSum;
    pointSum.x = point1.x + point2.x;
    pointSum.y = point1.y + point2.y;
    return pointSum;
}
/*
 * Overloaded operator * for GPoint
 */
GPoint operator * (GPoint &point, double &num) {
    GPoint pointMultiply;
    pointMultiply.x = point.x*num;
    pointMultiply.y = point.y*num;
    return pointMultiply;
}
GPoint operator * (double &num, GPoint &point) {
    GPoint pointMultiply;
    pointMultiply.x = point.x*num;
    pointMultiply.y = point.y*num;
    return pointMultiply;
}
/*
 * Draw a self similar fractal triangle
 */
int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint three, int order) {
    if (order==0) {
        fillBlackTriangle(window,one,two,three);
        return 1;
    }
    else {
        return drawSierpinskiTriangle(window,one,(one+two)*0.5,(one+three)*0.5,order-1) +
                drawSierpinskiTriangle(window,(one+two)*0.5,two,(two+three)*0.5,order-1) +
                drawSierpinskiTriangle(window,(one+three)*0.5,(two+three)*0.5,three,order-1);
    }

}


/* * * * * * Test Cases * * * * * */

/*
 * Do not modify the code beyond this line! There are no
 * unit tests for this problem. You should do all testing
 * via the interactive demo.
 */

PROVIDED_TEST("Test fractal drawing interactively using graphical demo") {

    runInteractiveDemo();
}

