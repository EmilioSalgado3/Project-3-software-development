/* Verse class function definitions
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Jesus Emilio Salgado Suarez
 */

#include "Verse.h"
#include "Ref.h"
#include <cstdlib>
#include <iostream>

using namespace std;

// Default constructor
Verse::Verse()
{  	
   verseText = "Uninitialized Verse!";
   verseRef = Ref();
} 

// REQUIRED: Parse constructor - pass verse string from file
Verse::Verse(const string s)
{
    string temp = s;

    // Extract book number
    string strbook = GetNextToken(temp, ":");
    int bookNum = atoi(strbook.c_str());

    // Extract chapter number
    string strchap = GetNextToken(temp, ":");
    int chapNum = atoi(strchap.c_str());

    // Extract verse number
    string strverse = GetNextToken(temp, " ");
    int versNum = atoi(strverse.c_str());

    // The rest of the line is the verse text
    verseText = temp;

    verseRef = Ref(bookNum, chapNum, versNum);
}


// REQUIRED: Accessors
string Verse::getVerse()
{
   return verseText;	   
}

Ref Verse::getRef()
{
   return verseRef;
}

// display reference and verse
void Verse::display()
{
   verseRef.display();
   cout << " " << verseText <<endl;
}