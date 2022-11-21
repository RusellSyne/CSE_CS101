#include "listnode.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;


void printList();
void deallocateList(ListNode* front);
ListNode* createList(Vector<int> values);
bool areEquivalent(ListNode* front, Vector<int> v);

/*
 * Select the first element of the linked list as the benchmark, and divide the linked list into three sub linked lists
 */
void partition(ListNode*& front, ListNode*& partitionOneFront,ListNode*& partitionTwoFront, ListNode*& partitionThreeFront) {
    if (front == nullptr)
        return;
    int base=front->data;
    ListNode* temp;
    while(front!=nullptr)
    {
        temp = front;
        front = front->next;
        if(temp->data<base) {
            temp->next = partitionOneFront;
            partitionOneFront = temp;
        }
        else if (temp->data==base) {
            temp->next = partitionTwoFront;
            partitionTwoFront = temp;
        }
        else {
            temp->next = partitionThreeFront;
            partitionThreeFront = temp;
        }

    }
}
/*
 * Connect the three linked lists end to end to form a linked list
 */
void concatenate(ListNode*& linkList, ListNode*& partitionOneFront,ListNode*& partitionTwoFront, ListNode*& partitionThreeFront) {

    linkList=partitionOneFront;
    if(linkList == nullptr)
        linkList = partitionTwoFront;
    else
    {
        while(partitionOneFront->next!=nullptr)
        {
            partitionOneFront=partitionOneFront->next;
        }
        partitionOneFront->next = partitionTwoFront;
    }

    if(linkList == nullptr)
        linkList = partitionThreeFront;
    else
    {
        while(partitionTwoFront->next!=nullptr)
        {
            partitionTwoFront=partitionTwoFront->next;
        }
        partitionTwoFront->next = partitionThreeFront;
    }
}
/*
 * Quick sort of a list
 */
void quickSort(ListNode*& front) {
    if(front==nullptr)
        return;
    ListNode* partitionOneFront = nullptr;
    ListNode* partitionTwoFront = nullptr;
    ListNode* partitionThreeFront = nullptr;
    if(front->next==nullptr) {
        partitionTwoFront=front;
        return;
    }
    partition(front,partitionOneFront,partitionTwoFront,partitionThreeFront);
    quickSort(partitionOneFront);
    quickSort(partitionThreeFront);
    concatenate(front,partitionOneFront,partitionTwoFront,partitionThreeFront);
}

/*
 * We have provided this utility function as a convenience. This
 * function traverses the linked list represented by the provided
 * front pointer and prints out the data stored in the list along
 * the way in a convenient format. You may find it to be helpful to
 * use this function as a debugging aid.
 */
void printList(ListNode* front) {
   cout << "{";
   for (ListNode *cur = front; cur != nullptr; cur = cur->next) {
       cout << cur->data;
       if (cur->next != nullptr){
           cout << ", ";
       }
   }
   cout << "}" << endl;
}

/*
 * This utility function deallocates the memory for all the nodes in a
 * given linked list. It can be used to recycle the memory allocated
 * during a test case and avoid memory leaks.
 */
void deallocateList(ListNode* front) {

    ListNode* next = front;
    while (front != nullptr) {
        front = front->next;
        delete next;
        next = front;
    }
}

/*
 * This utility function is given a vector of values and constructs
 * an equivalent linked list containing the same values in the same order.
 * It can be used to construct a linked list to use as an input to
 * a function being tested.
 */
ListNode* createList(Vector<int> values){
    ListNode* list = nullptr;
    for(int i=values.size()-1;i>=0;i--)
        list = new ListNode(values[i], list);
    return list;
}

/*
 * This utility function compares a linked list and a vector for
 * equivalence and returns true if both contain the same values in the
 * same order. It can be used to confirm a linked list's contents match
 * the expected.
 */
bool areEquivalent(ListNode* front, Vector<int> v){
    if (v.size()==0 || front == nullptr)
    {
        if (v.size()==0 && front == nullptr)
            return true;
        else
            return false;
    }

    for(int i=0;i<v.size();i++)
    {
        if(front==nullptr)
            return false;
        if(v[i]!=front->data)
            return false;
        front = front->next;
    }
    if(front!=nullptr)
        return false;
    return true;
}
/* * * * * * Test Code Below This Point * * * * * */

/* TODO: Write your own student tests below to test your sorting helper
 * functions and overall sorting algorithm correctness. We have provided
 * the skeleton of some tests below to help you get started with this
 * process.
 */

STUDENT_TEST("Test boundary conditions for 'areEquivalent'.") {
    ListNode* testList = nullptr;

    EXPECT(areEquivalent(testList, {}));
    EXPECT(!areEquivalent(testList, {1, 2, 3}));

    testList = new ListNode(3, testList);
    testList = new ListNode(2, testList);
    testList = new ListNode(1, testList);
    EXPECT(!areEquivalent(testList, {}));

    deallocateList(testList);
}

STUDENT_TEST("Test partition.") {
    ListNode* testList = nullptr;
    testList = new ListNode(3, testList);
    testList = new ListNode(2, testList);
    testList = new ListNode(1, testList);
    testList = new ListNode(5, testList);
    testList = new ListNode(6, testList);
    testList = new ListNode(4, testList);
    EXPECT(areEquivalent(testList, {4,6,5,1,2,3}));
    ListNode* partitionOneFront = nullptr;
    ListNode* partitionTwoFront = nullptr;
    ListNode* partitionThreeFront = nullptr;
    partition(testList,partitionOneFront,partitionTwoFront,partitionThreeFront);

    EXPECT(areEquivalent(partitionOneFront, {3,2,1}));
    EXPECT(areEquivalent(partitionTwoFront, {4}));
    EXPECT(areEquivalent(partitionThreeFront, {5,6}));


    deallocateList(testList);
    deallocateList(partitionOneFront);
    deallocateList(partitionTwoFront);
    deallocateList(partitionThreeFront);
}

STUDENT_TEST("Test boundary for partition - 0 element.") {
    ListNode* testList = nullptr;
    EXPECT(areEquivalent(testList, {}));
    ListNode* partitionOneFront = nullptr;
    ListNode* partitionTwoFront = nullptr;
    ListNode* partitionThreeFront = nullptr;
    partition(testList,partitionOneFront,partitionTwoFront,partitionThreeFront);

    EXPECT(areEquivalent(partitionOneFront, {}));
    EXPECT(areEquivalent(partitionTwoFront, {}));
    EXPECT(areEquivalent(partitionThreeFront, {}));


    deallocateList(testList);
    deallocateList(partitionOneFront);
    deallocateList(partitionTwoFront);
    deallocateList(partitionThreeFront);
}

STUDENT_TEST("Test boundary for partition - 1 element.") {
    ListNode* testList = nullptr;
    testList = new ListNode(3, testList);
    EXPECT(areEquivalent(testList, {3}));
    ListNode* partitionOneFront = nullptr;
    ListNode* partitionTwoFront = nullptr;
    ListNode* partitionThreeFront = nullptr;
    partition(testList,partitionOneFront,partitionTwoFront,partitionThreeFront);

    EXPECT(areEquivalent(partitionOneFront, {}));
    EXPECT(areEquivalent(partitionTwoFront, {3}));
    EXPECT(areEquivalent(partitionThreeFront, {}));


    deallocateList(testList);
    deallocateList(partitionOneFront);
    deallocateList(partitionTwoFront);
    deallocateList(partitionThreeFront);
}

STUDENT_TEST("Test concatenate.") {
    ListNode* linkList = nullptr;
    ListNode* one = nullptr;
    one = new ListNode(3, one);
    one = new ListNode(2, one);
    ListNode* two = nullptr;
    two = new ListNode(5, two);
    two = new ListNode(7, two);
    ListNode* three = nullptr;
    three = new ListNode(11, three);
    three = new ListNode(15, three);
    three = new ListNode(13, three);

    concatenate(linkList,one,two,three);

    EXPECT(areEquivalent(linkList, {2,3,7,5,13,15,11}));


    deallocateList(linkList);
}

STUDENT_TEST("Test boundary condition for concatenate - two empty lists.") {
    ListNode* linkList = nullptr;
    ListNode* one = nullptr;
    ListNode* two = nullptr;
    ListNode* three = nullptr;
    three = new ListNode(11, three);
    three = new ListNode(15, three);
    three = new ListNode(13, three);

    concatenate(linkList,one,two,three);

    EXPECT(areEquivalent(linkList, {13,15,11}));


    deallocateList(linkList);
}

STUDENT_TEST("Test boundary condition for concatenate - all empty lists.") {
    ListNode* linkList = nullptr;
    ListNode* one = nullptr;
    ListNode* two = nullptr;
    ListNode* three = nullptr;

    concatenate(linkList,one,two,three);

    EXPECT(areEquivalent(linkList, {}));


    deallocateList(linkList);
}

STUDENT_TEST("Sort empty list."){

    Vector<int> values = {};
    ListNode* list = createList(values);

    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);
}

STUDENT_TEST("Sort one-element list."){

    Vector<int> values = {3};
    ListNode* list = createList(values);

    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);
}

STUDENT_TEST("Sort list with inverse order."){

    Vector<int> values = {9,8,7,6,5,4,1};
    ListNode* list = createList(values);

    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);
}

STUDENT_TEST("Sort list with duplicate elements."){

    Vector<int> values = {9,8,7,6,5,4,1,8,9,5,6,7,8,9,5,4,7};
    ListNode* list = createList(values);

    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);
}

STUDENT_TEST("Sort list with same elements."){

    Vector<int> values = {6,6,6,6,6,6,6,6};
    ListNode* list = createList(values);

    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);
}

STUDENT_TEST("Sort list with negative numbers."){

    Vector<int> values = {-8,-9,-6,5,2,7,6,-5,-6,3,8};
    ListNode* list = createList(values);

    quickSort(list);
    values.sort();
    EXPECT(areEquivalent(list, values));
    deallocateList(list);
}

STUDENT_TEST("Sort list with random numbers."){
    int startSize = 50000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = randomInteger(-10000, 10000);
            list = new ListNode(v[i], list);
        }
        quickSort(list);
        v.sort();
        EXPECT(areEquivalent(list, v));
        deallocateList(list);
    }
}

//STUDENT_TEST("Sort list with critical length."){
//    int size = 5e8;

//    Vector<int> v(size);
//    for (int i = 1; i < size; i++) {
//        v[i] = randomInteger(-10000, 10000);
//    }
//    ListNode* list = createList(v);
//    quickSort(list);
//    v.sort();
//    EXPECT(areEquivalent(list, v));
//    deallocateList(list);

//}

PROVIDED_TEST("Time complexity") {
    int startSize = 50000;

    for(int n = startSize; n < 100*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = randomInteger(-10000, 10000);
            list = new ListNode(v[i], list);
        }
        TIME_OPERATION(n, quickSort(list));

        deallocateList(list);
    }
}

STUDENT_TEST("This is a skeleton to demonstrate an end-to-end student test."){
    /* Initialize vector to sequence of values */
    Vector<int> values = {1, 2, 3, 4};
    /* Create equivalent list to use as test input */
    ListNode* list = createList(values);

    /* Sort the list */
    quickSort(list);

    /* Sort the equivalent vector */
    values.sort();

    /* Confirm sorted list matches sorted vector */
    EXPECT(areEquivalent(list, values));


    /* Avoid memory leaks by deallocating list */
    deallocateList(list);
}

PROVIDED_TEST("This tests some of the testing helper functions you are recommended to implement."){
    /* Creates a small test list containing the values 1->2->3. */
    ListNode* testList = nullptr;
    testList = new ListNode(3, testList);
    testList = new ListNode(2, testList);
    testList = new ListNode(1, testList);

    /* Test equality comparison function. */
    EXPECT(areEquivalent(testList, {1, 2, 3}));
    EXPECT(!areEquivalent(testList, {1, 2}));
    EXPECT(!areEquivalent(testList, {1, 2, 3, 4}));
    EXPECT(!areEquivalent(testList, {1, 3, 2}));

    /* Test create list function. */
    ListNode* studentList = createList({1, 2, 3, 4});
    printList(studentList);
    ListNode* cur = studentList;
    for (int i = 1; i <= 4; i++){
        EXPECT(cur != nullptr);
        EXPECT_EQUAL(cur->data, i);
        cur = cur->next;
    }

    /* Test deallocate list functions. There should be no memory leaks from this test. */
    deallocateList(studentList);
    deallocateList(testList);
}

/*
 * The provided test case demonstrates a comparsion between the same
 * algorithm (quicksort) operating on a linked list versus a vector.
 */
PROVIDED_TEST("Time linked list quicksort vs vector quicksort") {
    int startSize = 50000;

    for(int n = startSize; n < 10*startSize; n *= 2) {
        Vector<int> v(n);
        ListNode* list = nullptr;

        /* Create linked list and vector with the same random sequence. */
        for (int i = n-1; i >= 0; i--) {
            v[i] = randomInteger(-10000, 10000);
            list = new ListNode(v[i], list);
        }

        /* NOTE: This test does not check correctness of the linked list sort
         * function. It only times the two operations to compare relative speed. */
        TIME_OPERATION(n, quickSort(list));
        TIME_OPERATION(n, v.sort());        /* Standard vector sort operation is backed
                                               with quicksort algorithm. */

        deallocateList(list);
    }
}

