#include "bits.h"
#include "treenode.h"
#include "huffman.h"
#include "map.h"
#include "vector.h"
#include "priorityqueue.h"
#include "strlib.h"
#include "testing/SimpleTest.h"
using namespace std;


EncodingTreeNode* createExampleTree();
/**
 * Given a Queue<Bit> containing the compressed message bits and the encoding tree
 * used to encode those bits, decode the bits back to the original message text.
 *
 * You can assume that tree is a well-formed non-empty encoding tree and
 * messageBits queue contains a valid sequence of encoded bits.
 *
 * Your implementation may change the messageBits queue however you like. There
 * are no requirements about what it should look like after this function
 * returns. The encoding tree should be unchanged.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
string decodeText(EncodingTreeNode* tree, Queue<Bit>& messageBits) {
    string s = "";
    EncodingTreeNode* temp = tree;
    while(!messageBits.isEmpty()) {
        if (messageBits.dequeue()==0)
            temp = temp->zero;
        else
            temp = temp->one;
        if(temp->isLeaf()) {
            s+=temp->ch;
            temp=tree;
        }
    }
    return s;
}

/**
 * Reconstruct encoding tree from flattened form Queue<Bit> and Queue<char>.
 *
 * You can assume that the queues are well-formed and represent
 * a valid encoding tree.
 *
 * Your implementation may change the queue parameters however you like. There
 * are no requirements about what they should look like after this function
 * returns.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
void unflattenTreeHelper(Queue<Bit>& treeShape, Queue<char>& treeLeaves, EncodingTreeNode* &tree) {
    if (treeShape.peek()==1) {
        tree = new EncodingTreeNode(nullptr,nullptr);
        treeShape.dequeue();
        unflattenTreeHelper(treeShape, treeLeaves, tree->zero);
        unflattenTreeHelper(treeShape, treeLeaves, tree->one);
    }
    else {
        tree=new EncodingTreeNode(treeLeaves.dequeue());
        treeShape.dequeue();
    }
}

EncodingTreeNode* unflattenTree(Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    if(treeShape.isEmpty())
        return nullptr;
    EncodingTreeNode* tree = nullptr;
    unflattenTreeHelper(treeShape, treeLeaves, tree);
    return tree;
}

/**
 * Decompress the given EncodedData and return the original text.
 *
 * You can assume the input data is well-formed and was created by a correct
 * implementation of compress.
 *
 * Your implementation may change the data parameter however you like. There
 * are no requirements about what it should look like after this function
 * returns.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
string decompress(EncodedData& data) {
    EncodingTreeNode* tree = unflattenTree(data.treeShape,data.treeLeaves);
    string s = decodeText(tree, data.messageBits);
    deallocateTree(tree);
    return s;
}

/**
 * Constructs an optimal Huffman coding tree for the given text, using
 * the algorithm described in lecture.
 *
 * Reports an error if the input text does not contain at least
 * two distinct characters.
 *
 * When assembling larger trees out of smaller ones, make sure to set the first
 * tree dequeued from the queue to be the zero subtree of the new tree and the
 * second tree as the one subtree.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
void calWeightEachChar(const string &text,  Map<char,double> &weightEachChar) {
    for(char elem:text) {
        if(!weightEachChar.containsKey(elem))
            weightEachChar.put(elem,1);
        else
            weightEachChar.put(elem,weightEachChar.get(elem)+1);
    }
}

void initialToPriorityQueue(Map<char,double> &weightEachChar, PriorityQueue<EncodingTreeNode*> &pq) {
    while(!weightEachChar.isEmpty()) {
        char tempKey = weightEachChar.firstKey();
        EncodingTreeNode* leafNode = new EncodingTreeNode(tempKey);
        pq.enqueue(leafNode,weightEachChar.get(tempKey));
        weightEachChar.remove(tempKey);
    }
}

void mergeQueue(PriorityQueue<EncodingTreeNode*> &pq, EncodingTreeNode*  &tree,EncodingTreeNode*  &zeroElem, EncodingTreeNode*  &oneElem,double &sumPriority) {
    sumPriority = pq.peekPriority();
    zeroElem =  pq.dequeue();
    if(pq.isEmpty())
        return;
    else {
        sumPriority  +=  pq.peekPriority();
        oneElem =  pq.dequeue();
        tree = new EncodingTreeNode(zeroElem,oneElem);
        pq.enqueue(tree,sumPriority);
        mergeQueue(pq, tree,zeroElem,oneElem,sumPriority);
    }
}

EncodingTreeNode* buildHuffmanTree(string text) {
    if(text.length()<2)
        error("The input text contains less than two distinct characters!");

    Map<char,double> weightEachChar;
    calWeightEachChar(text,weightEachChar);

    PriorityQueue<EncodingTreeNode*> pq;
    initialToPriorityQueue(weightEachChar,pq);
    EncodingTreeNode*  tree = nullptr;
    EncodingTreeNode*  zeroElem;
    EncodingTreeNode*  oneElem;
    double sumPriority;

    mergeQueue(pq, tree,zeroElem,oneElem,sumPriority);
    return tree;
}

/**
 * Given a string and an encoding tree, encode the text using the tree
 * and return a Queue<Bit> of the encoded bit sequence.
 *
 * You can assume tree is a valid non-empty encoding tree and contains an
 * encoding for every character in the text.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */

// helper function  for encodeText, find the sequence of each char.
void codingEachChar(EncodingTreeNode* &tree, Map<char, Queue<Bit>> &mapEachChar, Queue<Bit> path,Queue<Bit> path_copy) {
    if(tree->isLeaf()) {
        mapEachChar.put(tree->ch,path);
        return;
    }
    else {
        path_copy=path;
        path.enqueue(0);
        codingEachChar(tree->zero,mapEachChar,path,path_copy);
        path=path_copy;
        path.enqueue(1);
        codingEachChar(tree->one,mapEachChar,path,path_copy);
    }
}

// helper function  for encodeText
// add "Queue<Bit> b" to "Queue<Bit> a"
Queue<Bit> addQueue(Queue<Bit> a,Queue<Bit> b) {
    while(!b.isEmpty()) {
        a.enqueue(b.dequeue());
    }
    return a;
}

Queue<Bit> encodeText(EncodingTreeNode* tree, string text) {
    Queue<Bit> messageBits;
    Map<char, Queue<Bit>> mapEachChar;
    Queue<Bit> path;
    codingEachChar(tree,mapEachChar,path,path);
    for(char c:text) {
        messageBits  = addQueue(messageBits,  mapEachChar.get(c));
    }
    return messageBits;
}

/**
 * Flatten the given tree into a Queue<Bit> and Queue<char> in the manner
 * specified in the assignment writeup.
 *
 * You can assume the input queues are empty on entry to this function.
 *
 * You can assume tree is a valid well-formed encoding tree.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
void flattenTree(EncodingTreeNode* tree, Queue<Bit>& treeShape, Queue<char>& treeLeaves) {
    if(tree->isLeaf()) {
        treeShape.enqueue(0);
        treeLeaves.enqueue(tree->ch);
    }
    else {
        treeShape.enqueue(1);
        flattenTree(tree->zero,treeShape,treeLeaves);
        flattenTree(tree->one,treeShape,treeLeaves);
    }
}

/**
 * Compress the input text using Huffman coding, producing as output
 * an EncodedData containing the encoded message and flattened
 * encoding tree used.
 *
 * Reports an error if the message text does not contain at least
 * two distinct characters.
 *
 * TODO: Add any additional information to this comment that is necessary to describe
 * your implementation.
 */
EncodedData compress(string messageText) {
    EncodedData  data;
    EncodingTreeNode* tree  = buildHuffmanTree(messageText);
    Queue<Bit> messageBits = encodeText(tree, messageText);
    Queue<Bit> treeShape;
    Queue<char> treeLeaves;
    flattenTree(tree,treeShape, treeLeaves);
    data.messageBits=messageBits;
    data.treeShape=treeShape;
    data.treeLeaves=treeLeaves;
    deallocateTree(tree);
    return data;
}

/* * * * * * Testing Helper Functions Below This Point * * * * * */

EncodingTreeNode* createExampleTree() {
    /* Example encoding tree used in multiple test cases:
     *                *
     *              /   \
     *             T     *
     *                  / \
     *                 *   E
     *                / \
     *               R   S
     */
    EncodingTreeNode  *leafOne = new EncodingTreeNode('T');
    EncodingTreeNode  *leafTwo = new EncodingTreeNode('R');
    EncodingTreeNode  *leafThree = new EncodingTreeNode('S');
    EncodingTreeNode  *leafFour = new EncodingTreeNode('E');
    EncodingTreeNode *insideOne = new EncodingTreeNode(leafTwo, leafThree);
    EncodingTreeNode *insideTwo = new EncodingTreeNode(insideOne, leafFour);
    EncodingTreeNode *root = new EncodingTreeNode(leafOne, insideTwo);
    return root;
}

void deallocateTree(EncodingTreeNode* t) {
    if (t->isLeaf())
    {
        delete t;
        return;
    }
    else
    {
        EncodingTreeNode *copy= t;
        delete copy;
        deallocateTree(t->zero);
        deallocateTree(t->one);
    }


}

bool areEqual(EncodingTreeNode* a, EncodingTreeNode* b) {
    if (a==nullptr || b==nullptr) {
        if (a==nullptr && b==nullptr)
            return true;
        else
            return false;
    }
    if(a->isLeaf() ||  b->isLeaf()) {
        if (a->isLeaf() &&  b->isLeaf() && a->ch==b->ch)
            return true;
        else
            return false;
    }
    areEqual(a->zero,b->zero);
    areEqual(a->one,b->one);
    return true;
}

/* * * * * * Test Cases Below This Point * * * * * */

/* TODO: Write your own student tests. */
STUDENT_TEST("test createExampleTree") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);
    deallocateTree(tree);

}

STUDENT_TEST("test createExampleTree") {
    EncodingTreeNode  *One = new EncodingTreeNode('T');
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(areEqual(One, One));
    EXPECT(!areEqual(One, nullptr));
    EXPECT(areEqual(tree, tree));
    EXPECT(!areEqual(One, tree));
    deallocateTree(tree);
    deallocateTree(One);

}
STUDENT_TEST("unflattenTree, small example encoding tree") {
    Queue<Bit>  treeShape  = { 1,1,0,1,0,0,1,0,0 };
    Queue<char> treeLeaves = { 'F', 'L', 'E', 'R', 'A' };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    deallocateTree(tree);
}

STUDENT_TEST("TEST codingEachChar") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    Map<char, Queue<Bit>> mapEachChar;
    Queue<Bit> path;
    codingEachChar(tree,mapEachChar,path,path);
    Map<char, Queue<Bit>>  ref = {{'T', {0}},{'R', {1,0,0}},{'S', {1,0,1}},{'E', {1,1}},};
    EXPECT_EQUAL(mapEachChar,ref);
    deallocateTree(tree);
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("decodeText, small example encoding tree") {
    EncodingTreeNode* tree = createExampleTree(); // see diagram above
    EXPECT(tree != nullptr);

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(decodeText(tree, messageBits), "E");

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(decodeText(tree, messageBits), "SET");

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1}; // STREETS
    EXPECT_EQUAL(decodeText(tree, messageBits), "STREETS");

    deallocateTree(tree);
}

PROVIDED_TEST("unflattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  treeShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeLeaves = { 'T', 'R', 'S', 'E' };
    EncodingTreeNode* tree = unflattenTree(treeShape, treeLeaves);

    EXPECT(areEqual(tree, reference));

    deallocateTree(tree);
    deallocateTree(reference);
}

PROVIDED_TEST("decompress, small example input") {
    EncodedData data = {
        { 1, 0, 1, 1, 0, 0, 0 }, // treeShape
        { 'T', 'R', 'S', 'E' },  // treeLeaves
        { 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1 } // messageBits
    };

    EXPECT_EQUAL(decompress(data), "TRESS");
}

PROVIDED_TEST("buildHuffmanTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    EncodingTreeNode* tree = buildHuffmanTree("STREETTEST");
    EXPECT(areEqual(tree, reference));

    deallocateTree(reference);
    deallocateTree(tree);
}

PROVIDED_TEST("encodeText, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above

    Queue<Bit> messageBits = { 1, 1 }; // E
    EXPECT_EQUAL(encodeText(reference, "E"), messageBits);

    messageBits = { 1, 0, 1, 1, 1, 0 }; // SET
    EXPECT_EQUAL(encodeText(reference, "SET"), messageBits);

    messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1 }; // STREETS
    EXPECT_EQUAL(encodeText(reference, "STREETS"), messageBits);

    deallocateTree(reference);
}

PROVIDED_TEST("flattenTree, small example encoding tree") {
    EncodingTreeNode* reference = createExampleTree(); // see diagram above
    Queue<Bit>  expectedShape  = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> expectedLeaves = { 'T', 'R', 'S', 'E' };

    Queue<Bit>  treeShape;
    Queue<char> treeLeaves;
    flattenTree(reference, treeShape, treeLeaves);

    EXPECT_EQUAL(treeShape,  expectedShape);
    EXPECT_EQUAL(treeLeaves, expectedLeaves);

    deallocateTree(reference);
}

PROVIDED_TEST("compress, small example input") {
    EncodedData data = compress("STREETTEST");
    Queue<Bit>  treeShape   = { 1, 0, 1, 1, 0, 0, 0 };
    Queue<char> treeChars   = { 'T', 'R', 'S', 'E' };
    Queue<Bit>  messageBits = { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0 };

    EXPECT_EQUAL(data.treeShape, treeShape);
    EXPECT_EQUAL(data.treeLeaves, treeChars);
    EXPECT_EQUAL(data.messageBits, messageBits);
}

PROVIDED_TEST("Test end-to-end compress -> decompress") {
    Vector<string> inputs = {
        "HAPPY HIP HOP",
        "Nana Nana Nana Nana Nana Nana Nana Nana Batman"
        "Research is formalized curiosity. It is poking and prying with a purpose. – Zora Neale Hurston",
    };

    for (string input: inputs) {
        EncodedData data = compress(input);
        string output = decompress(data);

        EXPECT_EQUAL(input, output);
    }
}
