/* Demo server program for Bible lookup using AJAX/CGI interface
 * By James Skon, February 10, 2011
 * updated by Bob Kasper, January 2020
 * updated by Deborah Wilson, February 2026
 * Mount Vernon Nazarene University
 *
 * This sample program works using the cgicc AJAX library to
 * allow live communication between a web page and a program running on the
 * same server that hosts the web server.
 *
 * This program is run by a request from the associated html web document.
 * A Javascript client function invokes an AJAX request,
 * passing the input form data as the standard input string.
 *
 * The cgi.getElement function parses the input string, searching for the matching
 * field name, and returning a "form_iterator" object, which contains the actual
 * string the user entered into the corresponding field. The actual values can be
 * accessed by dereferencing the form iterator twice, e.g. **verse
 * refers to the actual string entered in the form's "verse" field.
 *
 * STUDENT NAME: Jesus Emilio Salgado Suarez
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>  // For stoi
#include "Bible.h"   // to create the bible object
#include "Ref.h"     // to create the reference object
#include "Verse.h"   // to create the verse object

/* Required libraries for AJAX to function */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"

using namespace std;
using namespace cgicc;

int main() {
   /* A CGI program must send a response header with content type
    * back to the web client before any other output.
    * For an AJAX request, our response is not a complete HTML document,
    * so the response type is just plain text to insert into the web page.
    */
   cout << "Content-Type: text/plain\n\n";

   Cgicc cgi;  // create object used to access CGI request data

   // GET THE INPUT DATA
   // browser sends us a string of field name/value pairs from HTML form
   // retrieve the value for each appropriate field name
   form_iterator st = cgi.getElement("search_type");
   form_iterator book = cgi.getElement("book");
   form_iterator chapter = cgi.getElement("chapter");
   form_iterator verse = cgi.getElement("verse");
   form_iterator nv = cgi.getElement("num_verse");

   // Convert and check input data
   bool validInput = true;  // Assume good until proven bad
   string searchType = (st != cgi.getElements().end()) ? **st : "";

   if (searchType != "reference") {
      cout << "<p>Only 'Verse lookup by reference' is implemented. Selected: " << searchType << "</p>" << endl;
      validInput = false;
   }

   // Book (input is number 1-66)
   int bookNum = 0;
   if (book == cgi.getElements().end()) {
      cout << "<p>Missing book input.</p>" << endl;
      validInput = false;
   } else {
      bookNum = book->getIntegerValue();
      if (bookNum < 1 || bookNum > 66) {
         cout << "<p>Invalid book number: must be between 1 and 66.</p>" << endl;
         validInput = false;
      }
   }

   // Chapter
   int chapterNum = 0;
   if (chapter == cgi.getElements().end() || (**chapter).empty()) {
      cout << "<p>Missing or empty chapter input.</p>" << endl;
      validInput = false;
   } else {
      try {
         chapterNum = stoi(**chapter);
         if (chapterNum <= 0) {
            cout << "<p>Chapter must be a positive number.</p>" << endl;
            validInput = false;
         } else if (chapterNum > 150) {
            cout << "<p>Invalid chapter: must be between 1 and 150.</p>" << endl;
            validInput = false;
         }
      } catch (...) {
         cout << "<p>Chapter must be a number.</p>" << endl;
         validInput = false;
      }
   }

   // Verse
   int verseNum = 0;
   if (verse == cgi.getElements().end() || (**verse).empty()) {
      cout << "<p>Missing or empty verse input.</p>" << endl;
      validInput = false;
   } else {
      try {
         verseNum = stoi(**verse);
         if (verseNum <= 0) {
            cout << "<p>Verse must be a positive number.</p>" << endl;
            validInput = false;
         } else if (verseNum > 176) {
            cout << "<p>Invalid verse: must be between 1 and 176.</p>" << endl;
            validInput = false;
         }
      } catch (...) {
         cout << "<p>Verse must be a number.</p>" << endl;
         validInput = false;
      }
   }

   // Number of verses (default to 1 if missing, empty, or invalid)
   int numVerses = 1;
   bool numVersesWarn = false;
   if (nv != cgi.getElements().end() && !(**nv).empty()) {
      try {
         numVerses = stoi(**nv);
         if (numVerses <= 0) {
            numVerses = 1;
            cout << "<p>Number of verses must be positive; defaulting to 1.</p>" << endl;
            numVersesWarn = true;
         }
      } catch (...) {
         cout << "<p>Number of verses must be a number; defaulting to 1.</p>" << endl;
         numVerses = 1;
         numVersesWarn = true;
      }
   }

   // If all inputs valid and search_type is reference, look up the verses
   string versesOutput = "";
   int versesFetched = 0;
   LookupResult status = OTHER;
   int currentBook = bookNum;
   int currentChapter = chapterNum;
   int currentVerse = verseNum;
   Ref firstRef(bookNum, chapterNum, verseNum);
   Ref lastRef;  // To be set after fetching
   if (validInput) {
      const string bibleFile = "/home/class/csc3004/Bibles/web-complete";  // Path to the Bible file
      Bible bibleObj(bibleFile);  // The bible object points to the bible file

      bool first = true;
      while (versesFetched < numVerses) {
         if (currentBook > 66) {
            break;  // End of Bible
         }
         Ref ref(currentBook, currentChapter, currentVerse);
         Verse v = bibleObj.lookup(ref, status);
         if (status == SUCCESS) {
            if (!first) {
               versesOutput += " ";  // Space between verses
            }
            versesOutput += v.getVerse();
            versesFetched++;
            lastRef = ref;  // Update last successful ref
            currentVerse++;
            first = false;
         } else if (status == NO_VERSE) {
            // End of chapter, move to next chapter
            currentChapter++;
            currentVerse = 1;
         } else if (status == NO_CHAPTER) {
            // End of book, move to next book
            currentBook++;
            currentChapter = 1;
            currentVerse = 1;
         } else {
            // NO_BOOK or OTHER
            if (versesFetched == 0) {
               cout << "<p>Lookup failed: " << bibleObj.error(status) << "</p>" << endl;
               validInput = false;
            }
            break;
         }
      }
   }

   /* SEND BACK THE RESULTS
    * Finally we send the result back to the client on the standard output stream
    * in HTML text format.
    * This string will be inserted as is inside a container on the web page,
    * so we must include HTML formatting commands to make things look presentable!
    */
   if (validInput && versesFetched > 0) {
      cout << "Search Type: <b>" << searchType << "</b>" << endl;
      cout << "<p>Your result: " << firstRef.getBookName() << " " << firstRef.getChapter() << ":" << firstRef.getVerse();
      if (versesFetched > 1) {
         cout << " - " << lastRef.getBookName() << " " << lastRef.getChapter() << ":" << lastRef.getVerse();
      }
      cout << " <em>" << versesOutput << "</em></p>" << endl;
   } else if (!validInput) {
      cout << "<p>Invalid Input: Please check the errors above and try again.</p>" << endl;
   } else if (numVersesWarn) {
      // If only numVerses warning but no other errors, still show if fetched
      if (versesFetched > 0) {
         cout << "Search Type: <b>" << searchType << "</b>" << endl;
         cout << "<p>Your result: " << firstRef.getBookName() << " " << firstRef.getChapter() << ":" << firstRef.getVerse();
         if (versesFetched > 1) {
            cout << " - " << lastRef.getBookName() << " " << lastRef.getChapter() << ":" << lastRef.getVerse();
         }
         cout << " <em>" << versesOutput << "</em></p>" << endl;
      }
   }

   return 0;
}