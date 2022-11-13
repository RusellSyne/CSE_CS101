#include "pqheap.h"
#include "error.h"
#include "random.h"
#include "strlib.h"
#include "datapoint.h"
#include "testing/SimpleTest.h"
using namespace std;

const int INITIAL_CAPACITY = 10;
const int NONE = -1; // used as sentinel index

/*
 * The constructor initializes all of the member variables needed for
 * an instance of the PQArray class. The allocated capacity
 * is initialized to a starting constant and a dynamic array of that
 * size is allocated. The number of filled slots is initially zero.
 */
PQHeap::PQHeap() {
    _numAllocated = INITIAL_CAPACITY;
    _elements = new DataPoint[_numAllocated](); // allocated zero'd memory
    _numFilled = 0;
}

/* The destructor is responsible for cleaning up any resources
 * used by this instance of the PQArray class. The array
 * memory that was allocated for the PQArray is deleted here.
 */
PQHeap::~PQHeap() {
    delete[] _elements;
}
//******************************helper functions start*********************//
//Reallocate memory for an array
void  PQHeap::extendArray() {
    DataPoint *old_elements = _elements;
    _numAllocated *= 2;
    _elements = new DataPoint[_numAllocated]();
    for (int i=0;i<size();i++) {
        _elements[i] = old_elements[i];
    }
    delete[] old_elements;
}
/*
 * Private member function. This helper exchanges the element at
 * indexA with the element at indexB.
 */
void PQHeap::swap(const int &indexA, const int &indexB) {
    DataPoint tmp = _elements[indexA];
    _elements[indexA] = _elements[indexB];
    _elements[indexB] = tmp;
}
//reset the order of the heap from the tail: index[_numFilled-1]
void  PQHeap::resetOrderFromTail(int &index, int &parentIndex) {
    if(parentIndex == NONE)
        return;
    if (_elements[index].priority>=_elements[parentIndex].priority)
        return;
    swap(index,parentIndex);
    index = parentIndex;
    parentIndex = getParentIndex(index);
    resetOrderFromTail(index,parentIndex);

}
//reset the order of the heap from the top: index[0]
void  PQHeap::resetOrderFromTop(int &index, int &leftChildIndex, int &rightChildIndex) {
    if(leftChildIndex == NONE)
        return;
    else if (leftChildIndex != NONE && rightChildIndex == NONE) {
        if(_elements[index].priority<=_elements[leftChildIndex].priority)
            return;
        else {
            swap(index,leftChildIndex);
            index = leftChildIndex;
        }
    }
    else {
        if (_elements[index].priority<=_elements[leftChildIndex].priority && _elements[index].priority<=_elements[rightChildIndex].priority)
            return;
        else if(_elements[index].priority>_elements[leftChildIndex].priority && _elements[leftChildIndex].priority<=_elements[rightChildIndex].priority) {
            swap(index,leftChildIndex);
            index = leftChildIndex;
        }
        else if(_elements[index].priority>_elements[rightChildIndex].priority && _elements[leftChildIndex].priority>_elements[rightChildIndex].priority) {
            swap(index,rightChildIndex);
            index = rightChildIndex;
        }

    }
    leftChildIndex = getLeftChildIndex(index);
    rightChildIndex = getRightChildIndex(index);
    resetOrderFromTop(index, leftChildIndex, rightChildIndex);

}
//******************************helper functions end*********************//
/*
 * Add an element to array
 */
void PQHeap::enqueue(DataPoint elem) {
    if(size()==_numAllocated)
        extendArray();
    _elements[size()] = elem;
    _numFilled++;
    int index = _numFilled-1;
    int parentIndex = getParentIndex(index);
    resetOrderFromTail(index, parentIndex);
}

/*
 * The array elements are stored in decreasing order of priority value.
 * The element at index 0 is the least urgent (largest priority value)
 * and the element in the last-filled index is the most urgent
 * (minimum priority value), this element is frontmost. peek returns
 * the frontmost element (most urgent, minimum priority value).
 */
DataPoint PQHeap::peek() const {
    if (isEmpty()) {
        error("PQueue is empty!");
    }
    return _elements[0];
}

/*
 * This function returns the value of the frontmost element and removes
 * it from the queue.  Because the frontmost element was at the
 * last-filled index, decrementing filled count is sufficient to remove it.
 */
DataPoint PQHeap::dequeue() {
    DataPoint front = peek();
    _elements[0] = _elements[_numFilled-1];
    _numFilled--;
    int index = 0;
    int leftChildIndex = getLeftChildIndex(index);
    int rightChildIndex = getRightChildIndex(index);
    resetOrderFromTop(index, leftChildIndex, rightChildIndex);
    return front;

}

/*
 * Returns true if no elements in the queue, false otherwise
 */
bool PQHeap::isEmpty() const {
    return size() == 0;
}

/*
 * The count of enqueued elements is tracked in the
 * member variable _numFilled.
 */
int PQHeap::size() const {
    return _numFilled;
}

/*
 * Updates internal state to reflect that the queue is empty, e.g. count
 * of filled slots is reset to zero. The array memory stays allocated
 * at current capacity. The previously stored elements do not need to
 * be cleared; the slots would be overwritten when additional elements
 * are enqueued, but as a defensive programming technique, we mark
 * each with a sentinel value as a debugging aid.
 */
void PQHeap::clear() {
    _numFilled = 0;
}

/*
 * Prints the contents of internal array for debugging purposes.
 */
void PQHeap::printDebugInfo(string msg) const {
    cout << msg << endl;
    for (int i = 0; i < size(); i++) {
        cout << "[" << i << "] = " << _elements[i] << endl;
    }
}

/*
 * We strongly recommend implementing this helper function, which
 * should traverse the heap array and ensure that the heap property
 * holds for all elements in the array. If elements are found that
 * violate the heap property, an error should be thrown.
 */
void PQHeap::validateInternalState() const {
    if (_numFilled > _numAllocated) error("Too many elements in not enough space!");
    for (int i = size() - 1; i >1; i--) {
        if (_elements[i].priority < _elements[(i-1)/2].priority) {
            printDebugInfo("validateInternalState");
            error("PQHeap has elements out of order at index " + integerToString(i));
        }
    }
}

/*
 * We strongly recommend implementing this helper function, which
 * calculates the index of the element that is the parent of the
 * specified child index. If this child has no parent, return
 * the sentinel value NONE.
 */
int PQHeap::getParentIndex(int child) const {
    if(child<1)
        return(NONE);
    else
        return (child-1)/2;
}

/*
 * We strongly recommend implementing this helper function, which
 * calculates the index of the element that is the left child of the
 * specified parent index. If this parent has no left child, return
 * the sentinel value NONE.
 */
int PQHeap::getLeftChildIndex(int parent) const {
    if(2*parent + 1 >= _numFilled)
        return(NONE);
    else
        return 2*parent + 1;
}

/*
 * We strongly recommend implementing this helper function, which
 * calculates the index of the element that is the right child of the
 * specified parent index. If this parent has no right child, return
 * the sentinel value NONE.
 */
int PQHeap::getRightChildIndex(int parent) const {
    if(2*parent + 2 >= _numFilled)
        return(NONE);
    else
        return 2*parent + 2;
}

/* * * * * * Test Cases Below This Point * * * * * */

STUDENT_TEST("PQHeap example from writeup") {
    PQHeap pq;

    pq.enqueue( { "Zoe", -3 } );
    pq.enqueue( { "Elmo", 10 } );
    pq.enqueue( { "Bert", 6 } );
    EXPECT_EQUAL(pq.size(), 3);
    pq.printDebugInfo("After enqueue 3 elements");

    pq.enqueue( { "Kermit", 5 } );
    EXPECT_EQUAL(pq.size(), 4);
    pq.printDebugInfo("After enqueue one more");

    DataPoint removed = pq.dequeue();
    DataPoint expected = { "Zoe", -3 };
    EXPECT_EQUAL(removed, expected);
    pq.printDebugInfo("After dequeue one");
}

STUDENT_TEST("PQHeap, enqueue only, validate at every step") {
    PQHeap pq;

    pq.enqueue({ "e", 2.718 });
    pq.validateInternalState();
    pq.enqueue({ "pi", 3.14 });
    pq.validateInternalState();
    pq.enqueue({ "phi", 1.618 });
    pq.validateInternalState();
    EXPECT_EQUAL(pq.size(), 3);
}

STUDENT_TEST("PQHeap: operations size/isEmpty/clear") {
    PQHeap pq;

    EXPECT(pq.isEmpty());
    pq.clear();
    EXPECT_EQUAL(pq.isEmpty(), pq.size() == 0);
    pq.enqueue({ "", 7 });
    EXPECT_EQUAL(pq.size(), 1);
    pq.enqueue({ "", 5 });
    EXPECT_EQUAL(pq.size(), 2);
    pq.enqueue({ "", 5 });
    EXPECT_EQUAL(pq.size(), 3);
    pq.clear();
    pq.validateInternalState();
    EXPECT(pq.isEmpty());
    EXPECT_EQUAL(pq.size(), 0);
}

STUDENT_TEST("PQHeap: dequeue or peek on empty queue raises error") {
    PQHeap pq;
    DataPoint point = { "Programming Abstractions", 106 };

    EXPECT(pq.isEmpty());
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());

    pq.enqueue(point);
    pq.dequeue();
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());

    pq.enqueue(point);
    pq.clear();
    EXPECT_ERROR(pq.dequeue());
    EXPECT_ERROR(pq.peek());
}

STUDENT_TEST("PQHeap, enqueue") {
    PQHeap pq;
    DataPoint point;
    pq.enqueue({ "e", 2.718 });
    point = pq.peek();
    EXPECT_EQUAL(point.label, "e");
    EXPECT_EQUAL(point.priority, 2.718);
    EXPECT_EQUAL(point, { "e", 2.718 });
    EXPECT_EQUAL(pq.peek(), { "e", 2.718 });
}

STUDENT_TEST("PQHeap, peak, validate at every step") {
    PQHeap pq;
    DataPoint point;
    pq.enqueue({ "e", 2.718 });
    EXPECT_EQUAL(pq.peek(), { "e", 2.718 });
    pq.enqueue({ "pi", 3.14 });
    EXPECT_EQUAL(pq.peek(), { "e", 2.718 });
    pq.enqueue({ "phi", 1.618 });
    EXPECT_EQUAL(pq.peek(), { "phi", 1.618 });
}

STUDENT_TEST("PQHeap, dequeue, validate at every step") {
    PQHeap pq;

    pq.enqueue({ "e", 2.718 });
    pq.enqueue({ "pi", 3.14 });
    pq.enqueue({ "phi", 1.618 });

    EXPECT_EQUAL(pq.dequeue(), { "phi", 1.618 });
    pq.validateInternalState();
    EXPECT_EQUAL(pq.dequeue(), { "e", 2.718 });
    pq.validateInternalState();
    EXPECT_EQUAL(pq.dequeue(), { "pi", 3.14 });
    pq.validateInternalState();
}

STUDENT_TEST("PQHeap, test enlarge array memory") {
    for (int size = 5; size <= 500; size *= 5) {
        PQHeap pq;

        for (int i = 1; i <= size; i++) {
            pq.enqueue({"", double(i) });
        }
        pq.validateInternalState();

        for (int i = 1; i <= size; i++) {
            DataPoint expected = {"", double(i) };
            EXPECT_EQUAL(pq.dequeue(), expected);
        }
    }
}

STUDENT_TEST("PQHeap, sequence of mixed operations") {
    PQHeap pq;
    int size = 30;
    double val = 0;

    for (int i = 0; i < size; i++) {
        pq.enqueue({"", --val });
    }
    val = 0;
    for (int i = 0; i < pq.size(); i++) {
        DataPoint front = pq.peek();
        EXPECT_EQUAL(pq.dequeue(), front);
        pq.enqueue( { "", ++val });
    }
    EXPECT_EQUAL(pq.size(), size);
    val = 0;
    while (!pq.isEmpty()) {
        DataPoint expected = { "", ++val };
        EXPECT_EQUAL(pq.dequeue(), expected);
    }
}


STUDENT_TEST("PQHeap stress test, cycle many random elements in and out") {
    PQHeap pq;
    int n = 0, maxEnqueues = 5000;
    double sumEnqueued = 0, sumDequeued = 0;

    setRandomSeed(42); // make test behavior deterministic

    DataPoint mostUrgent = { "", 0 };
    pq.enqueue(mostUrgent);
    while (true) {
        if (++n < maxEnqueues && randomChance(0.9)) {
            DataPoint elem = { "", randomInteger(-10, 10) + 0.5 };
            if (elem.priority < mostUrgent.priority) {
                mostUrgent = elem;
            }
            sumEnqueued += elem.priority;
            pq.enqueue(elem);
        } else {
            DataPoint elem = pq.dequeue();
            sumDequeued += elem.priority;
            EXPECT_EQUAL(elem, mostUrgent);
            if (pq.isEmpty()) break;
            mostUrgent = pq.peek();
        }
    }
    EXPECT_EQUAL(sumEnqueued, sumDequeued);
}

void fillQueue(PQHeap& pq, int n) {
    pq.clear(); // start with empty queue
    for (int i = 0; i < n; i++) {
        pq.enqueue({ "", randomReal(0, 10) });
    }
}

void emptyQueue(PQHeap& pq, int n) {
    for (int i = 0; i < n; i++) {
        pq.dequeue();
    }
}

PROVIDED_TEST("PQHeap timing test, fillQueue and emptyQueue") {
    PQHeap pq;
    int n = 20000;

    for (int i=0;i<4;i++)
    {

        TIME_OPERATION(n, fillQueue(pq, n));
        TIME_OPERATION(n, emptyQueue(pq, n));
        n*=2;
    }
}
///* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("PQHeap example from writeup, validate each step") {
    PQHeap pq;
    Vector<DataPoint> input = {
        { "R", 4 }, { "A", 5 }, { "B", 3 }, { "K", 7 }, { "G", 2 },
        { "V", 9 }, { "T", 1 }, { "O", 8 }, { "S", 6 } };

    pq.validateInternalState();
    for (DataPoint dp : input) {
        pq.enqueue(dp);
        pq.validateInternalState();
    }
    while (!pq.isEmpty()) {
        pq.dequeue();
        pq.validateInternalState();
    }
}
