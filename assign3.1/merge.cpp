/**
 * File: merge.cpp
 * ----------------
 *  Merge queues using iteration and recursion, respectively
 */
#include <iostream>
#include "queue.h"
#include "testing/SimpleTest.h"
#include <math.h>
using namespace std;

bool checkQueue(Queue<int> a)
{
    if (a.size()<=1)
    {
        return true;
    }
    while(a.size()>1)
    {
        if (a.dequeue()>a.peek())
            return false;
    }
    return true;
}
/*
 *The elements of the queue "a" and "b" have been increased from small to large.
 *The returned result is also a queue added from small to large,
 *containing all the elements in the queue one and two.
 */
Queue<int> binaryMerge(Queue<int> a, Queue<int> b) {
    if(!checkQueue(a))
    {
        error("The order of numbers in Queue 'a' is not from small to large");
    }
    if(!checkQueue(b))
    {
        error("The order of numbers in Queue 'b' is not from small to large");
    }
    Queue<int> result;
    while(!a.isEmpty() || !b.isEmpty())
    {
        if(a.isEmpty())
             result.enqueue(b.dequeue());
        else if(b.isEmpty())
             result.enqueue(a.dequeue());
        else
            (a.peek()<b.peek())? result.enqueue(a.dequeue()) : result.enqueue(b.dequeue());
    }
    return result;
}

/*
 * This function assumes correct functionality of the previously
 * defined binaryMerge function and makes use of this function to
 * iteratively merge a collection of sequences, starting off with
 * an empty sequence and progressively merging in one sequence at
 * a time. This function is provided for you fully implemented –
 * it does not need to modified at all.
 */
Queue<int> naiveMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;

    for (Queue<int>& q : all) {
        result = binaryMerge(q, result);
    }
    return result;
}

/*
 * Merge multiple queues in a recursive way
 */
Queue<int> recMultiMerge(Vector<Queue<int>>& all) {
    if (all.size()==0) {
        return {};
    }
    else if (all.size()==1) {
        return all[0];
    }
    else if (all.size()==2) {
        return binaryMerge(all[0], all[1]);
    }
    else {
        Vector<Queue<int>> left = all.subList(0,floor(all.size()/2));
        Vector<Queue<int>> right = all.subList(floor(all.size()/2),all.size()-floor(all.size()/2));
        return binaryMerge(recMultiMerge(left),recMultiMerge(right));
    }
}


/* * * * * * Test Cases * * * * * */

Queue<int> createSequence(int size);
void distribute(Queue<int> input, Vector<Queue<int>>& all);

PROVIDED_TEST("check the order of Queue") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 4, 3};
    //cout<<endl<<a.dequeue()<<endl;
    EXPECT(checkQueue(a));
    EXPECT(!checkQueue(b));
}

PROVIDED_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 2, 3, 3, 4, 5};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

STUDENT_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {};
    Queue<int> b = {};
    Queue<int> c = {2,1,3};
    Queue<int> d = {-1,2,5,8};
    Queue<int> e = {2, 4, 5};
    Queue<int> expected = {-1, 2, 2, 4, 5, 5,8};
    EXPECT_EQUAL(binaryMerge(a, b), {});
    EXPECT_ERROR(binaryMerge(b, c));
    EXPECT_EQUAL(binaryMerge(b, d), d);
    EXPECT_EQUAL(binaryMerge(d, e), expected);
}

PROVIDED_TEST("naiveMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

PROVIDED_TEST("recMultiMerge, compare to naiveMultiMerge") {
    int n = 20;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

PROVIDED_TEST("Time binaryMerge operation") {
    int n = 1000000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
}

PROVIDED_TEST("Time naiveMultiMerge operation") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), naiveMultiMerge(all));
}
PROVIDED_TEST("Time recMultiMerge operation") {
    int n = 90000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), recMultiMerge(all));
}


/* Test helper to fill queue with sorted sequence */
Queue<int> createSequence(int size) {
    Queue<int> q;
    for (int i = 0; i < size; i++) {
        q.enqueue(i);
    }
    return q;
}

/* Test helper to distribute elements of sorted sequence across k sequences,
   k is size of Vector */
void distribute(Queue<int> input, Vector<Queue<int>>& all) {
    while (!input.isEmpty()) {
        all[randomInteger(0, all.size()-1)].enqueue(input.dequeue());
    }
}
