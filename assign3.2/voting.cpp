/**
 * File: voting.cpp
 * ----------------
 * Compute Banzhaf Impact Index
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "voting.h"
#include "testing/SimpleTest.h"
#include <cmath>
#include "queue.h"
#include "map.h"
using namespace std;

//sum of all values in an int-type Vector
int sumVectorInt(const Vector<int> &v)
{
    int sum(0);
    for(int i=0;i<v.size();i++)
    {
        sum+=v[i];
    }
    return sum;
}

//Find the same value in a Vector
int findSameValue(const Vector<int> &v, const int &num) {
    if(v.size() == 0)
        return -1;
    else
    {
        for(int i=0;i<v.size();i++)
        {
            if(v[i]==num)
                return i;
        }
    }
    return -1;
}

#define version_2
//******************************************  version-1 ***************************************
//version 1 is the common method that the most of people used.
#ifdef version_1
/**
 * compute key votes for each block
 * ----------------
 * blocksNew: the blocks exclude the target block.
 * blocksNewSize: size of all the blocks except the target block
 * sumVotes: sum of all votes in the blocks except the target block.
 * numKeyVotes: calculate the key votes of the target block.
 * allVotes: all votes of all blocks.
 * curVotes: the votes of the target block.
 */
void keyVotesCalculate(const Vector<int> &blocksNew,const int & blockNewSize, int sumVotes, int& numKeyVotes,const int& halfVotes, int index,const int & curVotes)
{
    if (sumVotes>halfVotes)
        return;
    if (index == blockNewSize )
    {
        if(sumVotes + curVotes > halfVotes && sumVotes <= halfVotes )
            numKeyVotes++;
    }
    else
    {
        keyVotesCalculate(blocksNew, blockNewSize, sumVotes + blocksNew[index], numKeyVotes,halfVotes,index+1,curVotes);
        keyVotesCalculate(blocksNew, blockNewSize, sumVotes, numKeyVotes,halfVotes,index+1,curVotes);
    }

}

// Compute Banzhaf Impact Index
Vector<int> computePowerIndexes(Vector<int>& blocks)
{
    Vector<int> result(blocks.size(),0);
    Vector<int> keyVotesRecord(blocks.size(),0);  //number of key votes
    int halfVotes=sumVectorInt(blocks)/2;            //half of all votes
    int blocksNewSize = blocks.size()-1;            //size of all the blocks except the target block
    for (int i=0;i<blocks.size();i++)
    {
        Vector<int> subBlocks = blocks.subList(0,i);
        int index = findSameValue(subBlocks,blocks[i]);
        if(index!=-1)
        {
            keyVotesRecord[i]=keyVotesRecord[index];
        }
        else
        {
            //recursive solving
            Vector<int> blocksNew = blocks.subList(0,i)+blocks.subList(i+1);
            int numKeyVotes(0);

            keyVotesCalculate(blocksNew,blocksNewSize,0,numKeyVotes,halfVotes,0,blocks[i]);
            keyVotesRecord[i]=numKeyVotes;
        }
    }
    //compute Banzhaf Impact Index based on key votes
    double sumKeyVotes = sumVectorInt(keyVotesRecord);
    for(int i=0;i<keyVotesRecord.size();i++)
    {
        result[i] = floor(100*keyVotesRecord[i]/(sumKeyVotes));
    }

    return result;
}
#endif
//******************************************  version-2 ***************************************
//version-2 is quite efficient when the key votes of a lot of blocks are the same, while the votes of these blocks are not the same.
/**
 * I made some optimizations for version-2 based on version-1:
 * I found that if the votes of a block-a is smaller that block-b,
 * then the key votes of block-a must smaller or equal to block-b.
 * So, I sorted the blocks from large to small before calculate the power indexes.
 * Then if the key votes of a block at index[i] equals to the key votes of a block at index[i-1],
 * we can just quit the recursion.
 */
#ifdef version_2
//sort Vector from large to small
Vector<int> sortVector(Vector<int> v)
{
    v.sort();
    int vSize = v.size();
    Vector<int> vSort=v;
    for(int i=0;i<vSize;i++)
        vSort[i] = v[vSize-i-1];
    return vSort;
}
STUDENT_TEST("sort Vector") {
    Vector<int> blocks = {50, 49, 1,56};
    Vector<int> blocks2 = {50, 49, 1,56,18};
    EXPECT_EQUAL(sortVector(blocks), {56,50,49,1});
    EXPECT_EQUAL(sortVector(blocks2), {56,50,49,18,1});
}
//save the initial index and velue for blocks
Map<int,Queue<int>> saveIniOrder(const Vector<int> &v)
{
    Map<int,Queue<int>> order;
    for (int i=0;i<v.size();i++)
    {
        Queue<int> val;
        if(order.containsKey(v[i])) {
            val = order.get(v[i]);
            val.enqueue(i);
            order.put(v[i],val);
        }
        else {
            val.enqueue(i);
            order.put(v[i],val);
        }
    }
    return order;
}
STUDENT_TEST("test saveIniOrder") {
    Vector<int> blocks = {29,29,29,29,27,27,3,4,3,5,6,5};
    Map<int,Queue<int>> expected = {{29,{0,1,2,3}},{27,{4,5}},{3,{6,8}},{4,{7}},{5,{9,11}},{6,{10}}};
    EXPECT_EQUAL(saveIniOrder(blocks), expected);
}
/**
 * compute key votes for each block
 * ----------------
 * blocksNew: the blocks exclude the target block.
 * blocksNewSize: size of all the blocks except the target block
 * sumVotes: sum of all votes in the blocks except the target block.
 * numKeyVotes: calculate the key votes of the target block.
 * numKeyVotesPre: the key votes of the previous block".
 * allVotes: all votes of all blocks.
 * curVotes: the votes of the target block.
 */
void keyVotesCalculate(const Vector<int> &blocksNew,const int & blockNewSize, int sumVotes, int& numKeyVotes,const int& numKeyVotesPre,const int& halfVotes, int index,const int & curVotes)
{
    if (sumVotes>halfVotes || numKeyVotes==numKeyVotesPre)
        return;
    if (index == blockNewSize )
    {
        if(sumVotes + curVotes > halfVotes && sumVotes <= halfVotes )
            numKeyVotes++;
    }
    else
    {
        keyVotesCalculate(blocksNew, blockNewSize, sumVotes + blocksNew[index], numKeyVotes,numKeyVotesPre,halfVotes,index+1,curVotes);
        keyVotesCalculate(blocksNew, blockNewSize, sumVotes, numKeyVotes,numKeyVotesPre,halfVotes,index+1,curVotes);
    }

}
// Compute Banzhaf Impact Index
void computePowerIndexesForSortedBlocks(Vector<int>& blocks, Vector<int>& result)
{

    Vector<int> keyVotesRecord(blocks.size(),0);  //number of key votes
    int halfVotes=sumVectorInt(blocks)/2;            //half of all votes
    int blocksNewSize = blocks.size()-1;            //size of all the blocks except the target block
    for (int i=0;i<blocks.size();i++)
    {
        Vector<int> subBlocks = blocks.subList(0,i);
        int index = findSameValue(subBlocks,blocks[i]);
        if(index!=-1)
        {
            keyVotesRecord[i]=keyVotesRecord[index];
        }
        else
        {
            //recursive solving
            Vector<int> blocksNew = blocks.subList(0,i)+blocks.subList(i+1);
            int numKeyVotes(0);
            int numKeyVotesPre(0);
            if(i==0)
                numKeyVotesPre = 1e8;
            else
                numKeyVotesPre = keyVotesRecord[i-1];

            keyVotesCalculate(blocksNew,blocksNewSize,0,numKeyVotes,numKeyVotesPre,halfVotes,0,blocks[i]);
            keyVotesRecord[i]=numKeyVotes;
        }
    }
    //compute Banzhaf Impact Index based on key votes
    double sumKeyVotes = sumVectorInt(keyVotesRecord);
    for(int i=0;i<keyVotesRecord.size();i++)
    {
        result[i] = floor(100*keyVotesRecord[i]/(sumKeyVotes));
    }
}

Vector<int> computePowerIndexes(Vector<int>& blocks)  {
    Vector<int> resultSorted(blocks.size(),0);
    Vector<int> result(blocks.size(),0);
    Vector<int> blockSorted = sortVector(blocks);
    Map<int,Queue<int>> orderSaved = saveIniOrder(blocks);

    computePowerIndexesForSortedBlocks(blockSorted,resultSorted);
    //Find the initial order
    for(int i = 0;i<blockSorted.size();i++)
    {
        int elem = blockSorted[i];
        if(i>0 && (blockSorted[i-1]==elem)){}
        else
        {
            Queue<int> index = orderSaved.get(elem);
            while(!index.isEmpty())
            {
                result[index.dequeue()] = resultSorted[i];
            }
        }

    }

    return result;

}
#endif

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Test power index, blocks 50-49-1") {
    Vector<int> blocks = {50, 49, 1};
    Vector<int> expected = {60, 20, 20};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}



PROVIDED_TEST("Test power index, blocks Hempshead 1-1-3-7-9-9") {
    Vector<int> blocks = {1, 1, 3, 7, 9, 9};
    Vector<int> expected = {0, 0, 0, 33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-NY 55-38-39") {
    Vector<int> blocks = {55, 38, 29};
    Vector<int> expected = {33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-GA 55-38-16") {
    Vector<int> blocks = {55, 38, 16};
    Vector<int> expected = {100, 0, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks EU post-Nice") {
    // Estonia is one of those 4s!!
    Vector<int> blocks = {29,29,29,29,27,27,14,13,12,12,12,12,12,10,10,10,7,7,7,7,7,4,4,4,4,4,3};
    Vector<int> expected = {8, 8, 8, 8, 7, 7, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks EU post-Nice") {
    // Estonia is one of those 4s!!
    Vector<int> blocks = {29,29,29,29,27,27,14,13,12,12,12,12,12,10,10,10,7,7,7,7,7,4,4,4,4,4,3};
    Vector<int> expected = {8, 8, 8, 8, 7, 7, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
}

PROVIDED_TEST("Time power index operation") {
    Vector<int> blocks;
    for (int i = 0; i < 25; i++) {
        blocks.add(randomInteger(1, 10));
    }
    TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
}

STUDENT_TEST("Time power index operation") {

    for (int i = 0; i <= 10; i++) {
        Vector<int> blocks;
        for(int j = 0; j < 20 + i; j++)
            blocks.add(randomInteger(1, 10));
        TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
    }
}


