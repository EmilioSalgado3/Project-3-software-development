#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include "Bible.h"
#include "Ref.h"
#include "Verse.h"
#include "fifo.h"
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"

using namespace std;
using namespace cgicc;

void printResult(string searchType, Ref& firstRef, Ref& lastRef, int versesFetched, string versesText) {
    cout << "Search Type: <b>" << searchType << "</b>" << endl;
    cout << "<p>Your result: "
         << firstRef.getBookName() << " " << firstRef.getChapter() << ":" << firstRef.getVerse();

    if (versesFetched > 1) {
        cout << " - "
             << lastRef.getBookName() << " " << lastRef.getChapter() << ":" << lastRef.getVerse();
    }

    cout << " <em>" << versesText << "</em></p>" << endl;
}

int main() {
    // CGI header
    cout << "Content-Type: text/html\n\n";

    Cgicc cgi;

    // Read form data
    form_iterator st = cgi.getElement("search_type");
    form_iterator book = cgi.getElement("book");
    form_iterator chapter = cgi.getElement("chapter");
    form_iterator verse = cgi.getElement("verse");
    form_iterator nv = cgi.getElement("num_verse");

    string searchType = (st != cgi.getElements().end()) ? **st : "";

    // Only reference lookup is implemented
    if (searchType != "reference") {
        cout << "<p>Only verse lookup by reference is implemented.</p>" << endl;
        return 0;
    }

    // Check required fields
    if (book == cgi.getElements().end() ||
        chapter == cgi.getElements().end() ||
        verse == cgi.getElements().end()) {
        cout << "<p>Missing input.</p>" << endl;
        return 0;
    }

    int bookNum = book->getIntegerValue();
    int chapterNum = atoi((**chapter).c_str());
    int verseNum = atoi((**verse).c_str());

    int numVerses = 1;
    if (nv != cgi.getElements().end() && !(**nv).empty()) {
        numVerses = atoi((**nv).c_str());
        if (numVerses < 1) {
            numVerses = 1;
        }
    }

    // Basic checks
    if (bookNum < 1 || bookNum > 66) {
        cout << "<p>Invalid book number.</p>" << endl;
        return 0;
    }

    if (chapterNum < 1) {
        cout << "<p>Invalid chapter number.</p>" << endl;
        return 0;
    }

    if (verseNum < 1) {
        cout << "<p>Invalid verse number.</p>" << endl;
        return 0;
    }

    // Create pipes
    Fifo requestPipe("request");
    Fifo replyPipe("reply");

    Ref firstRef(bookNum, chapterNum, verseNum);
    Ref lastRef(bookNum, chapterNum, verseNum);

    string versesOutput = "";
    int versesFetched = 0;

    int currentBook = bookNum;
    int currentChapter = chapterNum;
    int currentVerse = verseNum;

    while (versesFetched < numVerses) {
        if (currentBook > 66) {
            break;
        }

        // Send one request
        string request = to_string(currentBook) + ":" +
                         to_string(currentChapter) + ":" +
                         to_string(currentVerse);

        requestPipe.openwrite();
        requestPipe.send(request);
        requestPipe.fifoclose();

        // Read one reply
        replyPipe.openread();
        string reply = replyPipe.recv();
        replyPipe.fifoclose();

        // Split status|text
        size_t pos = reply.find('|');
        if (pos == string::npos) {
            cout << "<p>Invalid server reply.</p>" << endl;
            return 0;
        }

        int status = atoi(reply.substr(0, pos).c_str());
        string text = reply.substr(pos + 1);

        if (status == SUCCESS) {
            if (versesFetched > 0) {
                versesOutput += " ";
            }

            versesOutput += text;
            versesFetched++;
            lastRef = Ref(currentBook, currentChapter, currentVerse);
            currentVerse++;
        }
        else {
            // First request failed -> show error
            if (versesFetched == 0) {
                cout << "<p>" << text << "</p>" << endl;
                return 0;
            }

            // Stop range if next verse does not exist
            break;
        }
    }

    if (versesFetched > 0) {
        printResult(searchType, firstRef, lastRef, versesFetched, versesOutput);
    } else {
        cout << "<p>No verses found.</p>" << endl;
    }

    return 0;
}