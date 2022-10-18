/*
* File: search.cpp
* ----------------------
* This cpp file can give all the websites that contact the keywords we input.
*/
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "strlib.h"
#include "vector.h"
#include "testing/SimpleTest.h"
#include "simpio.h"
using namespace std;

/*
* cleanToken is used to clean a string.
* If all the characters in this string are non-alphabetic, then return a empty string.
* The punctuation at the beginning and at the end of the string will be delete.
* Uppercase letters are converted to lowercase letters
*/
string cleanToken(string s)
{
    while(s.size()!=0)
    {
        if (ispunct(s.front()))
            s.erase(s.begin());
        else
            break;
    }
    while(s.size()!=0)
    {
        if (ispunct(s.back()))
        {
            //s.erase(s.end()); //cannot work,why?
            s.pop_back();
        }

        else
            break;
    }
    int flagAplha=0;
    for(int i=0;i<s.size();i++)
    {
        if(isalpha(s[i]))
        {
            s[i]=tolower(s[i]);
            flagAplha=1;
        }
    }
    if (flagAplha==1)
        return s;
    else
        return "\0";
}


/*
 * Get all the strings from texts and return a Set<string> datatype.
*/
Set<string> gatherTokens(string text)
{
    Set<string> tokens;
    Vector<string> textVector = stringSplit(text, " ");
    for(int i=0;i<textVector.size();i++)
    {
        string textVectorClean=cleanToken(textVector[i]);
        if (!textVectorClean.empty())
            tokens.add(textVectorClean);

    }
    return tokens;
}

/*
 * Read all the contents from a file and output a vector.
*/
Vector<string> readWebFile(string filename) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    return lines;
}
/*
*Read the content from file and process it as a reverse index.
*/
int buildIndex(string dbfile, Map<string, Set<string>>& index)
{
    Vector<string> webText=readWebFile(dbfile);
    int numWeb=0;
    for(int i=0;i<webText.size();i+=2)
    {
        numWeb++;
        string webURL=webText[i];
        string webContent=webText[i+1];
        Set<string> webToken=gatherTokens(webContent);
        while (!webToken.isEmpty())
        {
            string key=webToken.last();
            Set<string> value;
            if (index.containsKey(key))
            {
                value = index.get(key);
            }
            value.add(webURL);
            index.put(key,value);
            webToken.remove(key);

        }
    }

    return numWeb;
}

/*
*Find all the mached websites for a input string.
*/
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query)
{
    Set<string> result;
    Vector<string> queryVector = stringSplit(query, " ");
    if (startsWith(queryVector[0],"+") || startsWith(queryVector[0],"-"))
    {
        error("The first word can not start with +/- !");
    }
    string queryTokenI;
    string queryStartI;
    for(int i=0;i<queryVector.size();i++)
    {
        queryTokenI=cleanToken(queryVector[i]);
        if(queryTokenI.empty())
        {
            error("The query word can not be empty!");
        }
        queryStartI=queryVector[i].at(0);
        if(queryStartI=="+")
        {
            result.intersect(index.get(queryTokenI));
        }
        else if(queryStartI=="-")
        {
            result.difference(index.get(queryTokenI));
        }
        else
        {
            result.unionWith(index.get(queryTokenI));
        }

    }
    return result;
}

/*
*For a input string, find all the mached websites from a given file.
*/
void searchEngine(string dbfile)
{
    Map<string, Set<string>> index;
    int numPages = buildIndex(dbfile, index);
    cout<<"Stand by while building index..."<<endl;
    cout<<"Indexed "<< numPages << " pages containing "<< index.size() <<" unique terms"<<endl<<endl;
    while(true)
    {
        string query = getLine("Enter query sentence (RETURN/ENTER to quit): ");
        if (query=="")
        {
            break;
        }
        Set<string> result = findQueryMatches(index,query);
        cout<<"Found " <<result.size()<< " matching pages"<<endl;
        cout<<result<<endl<<endl;
    }

}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings with no punctuation at beginning or end") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS*106B"), "cs*106b");
}

PROVIDED_TEST("cleanToken on strings with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
    EXPECT_EQUAL(cleanToken("fish,"), "fish");
}

PROVIDED_TEST("cleanToken on non-word strings (no letters)"){
    EXPECT_EQUAL(cleanToken("106"), "");
    EXPECT_EQUAL(cleanToken("~!106!!!"), "");
}



PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs*106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 5 unique words, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");

    EXPECT_EQUAL(tokens.size(), 5);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}



PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 11 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 11);
    EXPECT(index.containsKey("fish"));
    EXPECT(index.containsKey("eat"));
    Map<string, Set<string>> indexFish;
    indexFish.put("fish",{"www.shoppinglist.com","www.dr.seuss.net","www.bigbadwolf.com"});
    EXPECT(index.containsKey("eat"));
}



PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT(matchesRedAndFish.contains("www.dr.seuss.net"));
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}




// TODO: add your test cases here
STUDENT_TEST("cleanToken on strings with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("fish,"), "fish");
    //EXPECT_EQUAL(cleanToken("fish  "), "fish");
}

STUDENT_TEST("gatherTokens from complex string") {
    Set<string> expected = {"milk", "fish", "bread", "5lb_m&ms"};
    EXPECT_EQUAL(gatherTokens("milk, fish, bread,    5lb_m&ms"), expected);
}

STUDENT_TEST("buildIndex from tiny.txt, 4 pages, 11 unique tokens") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Map<string, Set<string>> indexFish;
    indexFish.put("fish",{"www.shoppinglist.com","www.dr.seuss.net","www.bigbadwolf.com"});
    EXPECT(index.containsKey("eat"));
    EXPECT_EQUAL(index.get("fish"),indexFish.get("fish"));

}

STUDENT_TEST("Time trials of buildIndex on INPUT files") {
    Map<string, Set<string>> index1;
    Map<string, Set<string>> index2;
    TIME_OPERATION("res/tiny.txt", buildIndex("res/tiny.txt", index1));
    TIME_OPERATION("res/website.txt", buildIndex("res/website.txt", index2));
}

STUDENT_TEST("findQueryMatches from website.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/website.txt", index);
    Set<string> matchesLecture = findQueryMatches(index, "lecture");
    EXPECT(matchesLecture.contains("http://cs106b.stanford.edu/class/cs106b/lectures/03-strings/"));

}

STUDENT_TEST("findQueryMatches from website.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
    EXPECT_EQUAL(matchesRedWithoutFish.first(), "www.rainbow.org");

}

