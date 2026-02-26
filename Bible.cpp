/* Bible class function definitions
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Jesus Emilio Salgado Suarez
 */

#include "Ref.h"
#include "Verse.h"
#include "Bible.h" 
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// Default constructor
Bible::Bible()
{ 
   infile = "/home/class/csc3004/Bibles/web-complete";
   isOpen = false;

   buildIndex();
}

// Constructor – pass bible filename
Bible::Bible(const string s) { 
   infile = s; 
   isOpen = false;

   buildIndex();
}

void Bible::buildIndex()
{
   instream.open(infile.c_str(), ios::in);
   if (!instream) {
      cerr << "Error - can't open input file for indexing: " << infile << endl;
      return;
   }

   string line;
   int offset = instream.tellg();

   while (getline(instream, line)) {
      Ref r(line);
      index[r] = offset;
      offset = instream.tellg();
   }

   instream.close();
   isOpen = false;

   cout << "Index built: " << index.size() << " verses" << endl;
}

Verse Bible::lookup(Ref ref, LookupResult& status)
{ 
   auto it = index.find(ref);
   if (it != index.end()) {

      currentPos = it;

      instream.open(infile.c_str(), ios::in);
      instream.seekg(it->second);

      string line;
      getline(instream, line);
      instream.close();

      status = SUCCESS;
      return Verse(line);
   }
   
    if (isOpen) {
       instream.close();
       isOpen = false;
   }

   instream.open(infile.c_str(), ios::in);

   if (!instream)
   {
       status = OTHER;
       cerr << "Error - can't open input file: " << infile << endl;
       return Verse();
   }//check if the file opens
    else
   {
       isOpen = true;
   }

    bool sawBook = false;
    bool sawChapter = false;

    int targetBook = ref.getBook();
    int targetChapter = ref.getChapter();
    int targetVerse = ref.getVerse();
    
    if (targetBook < 1 || targetBook > 66)
    {
        status = NO_BOOK;
        instream.close();
        isOpen = false;
        return Verse();
    }

    if (targetChapter < 1)
    {
            status = NO_CHAPTER;
            instream.close();
            isOpen = false;
            return Verse();
    }

    if (targetVerse < 1)
    {
            status = NO_VERSE;
            instream.close();
            isOpen = false;
            return Verse();
    }

    
    string line;
    while (getline(instream, line))// this loop is what looks for the verse
    {
            Ref current(line);

            if (current.getBook() > targetBook) {
                if (!sawBook)
                    status = NO_BOOK;
                else if (!sawChapter)
                    status = NO_CHAPTER;
                else
                    status = NO_VERSE;

                instream.close();
                isOpen = false;
                return Verse();
            }

            if (current.getBook() < targetBook) {
                continue;
            }

            sawBook = true;

            if (current.getChapter() < targetChapter) {
                continue;
            }

            if (current.getChapter() > targetChapter) {
                status = sawChapter ? NO_VERSE : NO_CHAPTER;
                instream.close();
                isOpen = false;
                return Verse();
            }

            if (current.getChapter() == targetChapter) {
                sawChapter = true;
                if (current.getVerse() < targetVerse) {
                    continue;
                }
                if (current.getVerse() > targetVerse) {
                    status = NO_VERSE;
                    instream.close();
                    isOpen = false;
                    return Verse();
                }

                status = SUCCESS;
                return Verse(line);
            }
    }

    if (!sawBook)
    {
        status = NO_BOOK;
    }
    else if (!sawChapter)
    {
        status = NO_CHAPTER;
    }
    else
    {
        status = NO_VERSE;
    };
    
    instream.close();
   isOpen = false;
   return Verse();
}

Verse Bible::nextVerse(LookupResult& status)
{
    if (currentPos == index.end()) {
        status = OTHER;
        return Verse();
    }

    currentPos++;

    if (currentPos == index.end()) {
        status = OTHER;
        return Verse();
    }

    int offset = currentPos->second;

    instream.open(infile.c_str(), ios::in);
    instream.seekg(offset);

    string line;
    getline(instream, line);
    instream.close();

    status = SUCCESS;
    return Verse(line);
}

string Bible::error(LookupResult status)
{
   if (status == SUCCESS)
       return "Verse was found";
   else if (status == NO_BOOK)
       return "The book doesnt exist in the Bible";
   else if (status == NO_CHAPTER)
       return "This chapter doesnt exist in this book";
   else if (status == NO_VERSE)
       return "This verse doesnt exist in this book";
   else if (status == OTHER)
       return "I have no idea what you did but thats awesome!";
   
   return "";
}

void Bible::display()
{
   cout << "Bible file: " << infile << endl;
}

Ref Bible::next(const Ref ref, LookupResult& status)
{
   return ref;
}

Ref Bible::prev(const Ref ref, LookupResult& status)
{
   return ref;
}
