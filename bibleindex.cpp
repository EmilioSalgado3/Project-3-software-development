/* Lookup Client (CGI) for Project 3 Part 2 - Bible Text Retrieval with Indexing
 * This is the new CGI program that replaces the old bibleajax.cpp.
 * It is invoked by the web interface (bibleindex.html) on every AJAX request.
 *
 * What it does:
 *   1. Reads the form data (book, chapter, verse, num_verse, etc.) just like Project 2.
 *   2. Instead of creating a Bible object and calling lookup() directly (which rebuilds the index every time),
 *      it sends the reference over the "request" pipe to the background Lookup Server.
 *   3. Receives the reply (status|verseText) over the "reply" pipe.
 *   4. Supports multiple verses (num_verse > 1) by sending multiple quick requests to the server.
 *   5. Builds the exact same HTML response string as the old version so the web page works unchanged.
 *
 * This uses the same pipe protocol as the Shakespeare demo but with Bible references.
 *
 * STUDENT NAME: Jesus Emilio Salgado Suarez
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>  // For stoi

/* We need Bible.h for the LookupResult enum and its values (SUCCESS, NO_BOOK, etc.) */
#include "Bible.h"   // ← THIS WAS MISSING - this fixes all your compiler errors
#include "Ref.h"     // Needed for firstRef / lastRef and getBookName()

/* Fifo for inter-process communication with the Lookup Server */
#include "fifo.h"

/* Required libraries for AJAX to function */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"

using namespace std;
using namespace cgicc;

int main() {
    /* Every CGI must send this header first so the browser knows it's plain text for AJAX */
    cout << "Content-Type: text/plain\n\n";

    Cgicc cgi;  // parse the incoming form data from the browser

    // GET THE INPUT DATA (exactly the same as Project 2)
    form_iterator st = cgi.getElement("search_type");
    form_iterator book = cgi.getElement("book");
    form_iterator chapter = cgi.getElement("chapter");
    form_iterator verse = cgi.getElement("verse");
    form_iterator nv = cgi.getElement("num_verse");

    bool validInput = true;
    string searchType = (st != cgi.getElements().end()) ? **st : "";

    if (searchType != "reference") {
        cout << "<p>Only 'Verse lookup by reference' is implemented. Selected: " << searchType << "</p>" << endl;
        validInput = false;
    }

    // Book validation (1-66)
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

    // Chapter validation
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

    // Verse validation
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

    // Number of verses (default 1)
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

    // If input is valid, communicate with the Lookup Server via pipes
    string versesOutput = "";
    int versesFetched = 0;
    LookupResult status = OTHER;          // ← Now recognized because of #include "Bible.h"
    int currentBook = bookNum;
    int currentChapter = chapterNum;
    int currentVerse = verseNum;
    Ref firstRef(bookNum, chapterNum, verseNum);
    Ref lastRef(0, 0, 0);   // will be updated only on successful lookups

    if (validInput) {
        // Create the two named pipes (same names the server uses)
        Fifo requestPipe("request");
        Fifo replyPipe("reply");

        bool first = true;

        while (versesFetched < numVerses) {
            if (currentBook > 66) {
                break;  // reached end of Bible
            }

            // Build the request string exactly as the server expects: "book:chapter:verse"
            string reqStr = to_string(currentBook) + ":" +
                            to_string(currentChapter) + ":" +
                            to_string(currentVerse);

            // === SEND REQUEST TO SERVER ===
            requestPipe.openwrite();
            requestPipe.send(reqStr);
            requestPipe.fifoclose();

            // === RECEIVE REPLY FROM SERVER ===
            replyPipe.openread();
            string replyMsg = replyPipe.recv();
            replyPipe.fifoclose();

            // Parse the reply "status|text"
            string verseText;   // ← declare here so it's visible in the error case too
            size_t sep = replyMsg.find('|');
            if (sep == string::npos) {
                status = OTHER;
                verseText = "Invalid server reply";
            } else {
                string statStr = replyMsg.substr(0, sep);
                verseText = replyMsg.substr(sep + 1);
                status = static_cast<LookupResult>(stoi(statStr));

                // Now handle the result exactly like the old bibleajax.cpp
                if (status == SUCCESS) {
                    if (!first) {
                        versesOutput += " ";
                    }
                    versesOutput += verseText;
                    versesFetched++;
                    lastRef = Ref(currentBook, currentChapter, currentVerse);
                    currentVerse++;
                    first = false;
                } else if (status == NO_VERSE) {
                    // End of chapter -> move to next chapter
                    currentChapter++;
                    currentVerse = 1;
                } else if (status == NO_CHAPTER) {
                    // End of book -> move to next book
                    currentBook++;
                    currentChapter = 1;
                    currentVerse = 1;
                } else {
                    // NO_BOOK or OTHER error
                    if (versesFetched == 0) {
                        cout << "<p>Lookup failed: " << verseText << "</p>" << endl;
                        validInput = false;
                    }
                    break;
                }
            }
        }
    }

    /* SEND BACK THE RESULTS TO THE WEB PAGE (same format as Project 2) */
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