/* Ref class function definitions
 * Computer Science, MVNU
 * CSC-3004 Introduction to Software Development
 *
 * NOTE: You may add code to this file, but do not
 * delete any code or delete any comments.
 *
 * STUDENT NAME: Jesus Emilio Salgado Suarez
 */

#include "Ref.h"
#include <iostream>
#include <string>

using namespace std;

static const string books[66] = {
   "Genesis", "Exodus", "Leviticus", "Numbers", "Deuteronomy", 
   "Joshua", "Judges", "Ruth", "1 Samuel", "2 Samuel", 
   "1 Kings", "2 Kings", "1 Chronicles", "2 Chronicles", "Ezra", 
   "Nehemiah",  "Esther", "Job", "Psalms", "Proverbs", 
   "Ecclesiastes", "Song of Solomon", "Isaiah", "Jeremiah", "Lamentations", 
   "Ezekiel", "Daniel", "Hosea", "Joel", "Amos", 
   "Obadiah", "Jonah", "Micah", "Nahum", "Habakkuk", 
   "Zephaniah", "Haggai", "Zechariah", "Malachi", 
   "Matthew", "Mark", "Luke", "John", 
   "Acts", "Romans",  "1 Corinthians",  "2 Corinthians", 
   "Galatians",  "Ephesians",  "Philippians",  "Colossians",
   "1 Thessalonians",  "2 Thessalonians",  "1 Timothy",  "2 Timothy",
   "Titus",  "Philemon",  "Hebrews",  "James",
   "1 Peter",  "2 Peter",  "1 John",  "2 John",
   "3 John",  "Jude",  "Revelation"
};//chaned it here because if it was muiltiple verses it would print again

/* GetNextToken - general routine for getting next token from a string
 * Parameters
 *    str - string to search. search is destructive, i.e., token is removed.
 *    delimiters - string, a list of characters to use a delimiters between tokens
 * Returns: string token (removed from str)
 */
string GetNextToken(string& str, const string& delimiters)
{
   // Skip delimiters at beginning
   string::size_type startPos = str.find_first_not_of(delimiters, 0);
   if (startPos == string::npos) {
      string next = "";
      str = "";
      return next;
   }

   // Find position of delimiter at end of token
   string::size_type endPos = str.find_first_of(delimiters, startPos);

   // If no further delimiter, the rest of the string is the token
   if (endPos == string::npos) {
      string next = str.substr(startPos);
      str = "";
      return next;
   }

   // Found a token, remove it from string, and return it
   string next = str.substr(startPos, endPos - startPos);
   string rest = str.substr(endPos + 1);
   str = rest;
   return next;
}

// Ref member functions

// Default constructor
Ref::Ref() {book = 0; chapter = 0; verse = 0;}  	

// Parse constructor - receives a line "34:5:7 text"
Ref::Ref(const string s)
{ 
   string rtext = s; // make local copy of string to avoid modifying parameter
   // parse the reference - notice, currently there is no error checking!
    
   // Get book number
   string strbook = GetNextToken(rtext,":");
   book = atoi(strbook.c_str());
    
   // Get the chapter number
   string strchap = GetNextToken(rtext,":");
   chapter = atoi(strchap.c_str());
    
   // Get the verse number
   string strverse = GetNextToken(rtext," ");
   verse = atoi(strverse.c_str());
}

// Construct Ref from three integers
Ref::Ref(const int b, const int c, const int v) 
{ 	
   book = b;
   chapter = c;
   verse = v;
}

// Accessors
int Ref::getBook() {return book;}	     // Access book number
int Ref::getChapter() {return chapter;}	 // Access chapter number
int Ref::getVerse() {return verse;};     // Access verse number


// REQUIRED: == comparison
bool Ref::operator==(const Ref & ref) const
{
   // TODO: implement comparison functions
  if (this->book == ref.book && this->chapter == ref.chapter &&
   this->verse == ref.verse)
   {return true;}
   
   return false;
}

// OPTIONAL: define < and > comparisons
bool Ref::operator< (const Ref & ref) const
{
   if (book < ref.book) return true;
   if (book > ref.book) return false;
   if (chapter < ref.chapter) return true;
   if (chapter > ref.chapter) return false;
   return verse < ref.verse;
}

bool Ref::operator> (const Ref & ref) const
{
   return ref < *this;
}

//get book name
string Ref::getBookName() const
{
   if (book < 1 || book > 66)
   {
      return "Invalid Book";
   }
   
   return books[book - 1];
}
// Display Reference
void Ref::display()
{ 	
   // TODO: modify display function to show book name instead of book number
      
   if (book < 1 || book > 66)
   {
      cout << "Invalid Book";
      return;
   }
   
	cout << getBookName() << " " << chapter << ":" << verse;
}

int Ref::getBookNum(const string& name) {
   for (int i = 0; i < 66; i++) {
      if (books[i] == name) {
         return i + 1;
      }
   }
   return 0;  // Invalid
}