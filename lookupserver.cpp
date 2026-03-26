/* Lookup Server for Project 3 Part 2 - Bible Text Retrieval with Indexing
 * Uses your indexed Bible class + pipes (Fifo)
 * This is the background server process that:
 *   1. Creates the Bible object (which builds the inverted index ONCE)
 *   2. Opens the request pipe for reading and waits for client requests
 *   3. For each request (a string "book:chapter:verse"):
 *        - Parses it into a Ref
 *        - Calls Bible::lookup (fast index lookup if possible)
 *        - Sends back a reply in the format "status|verseTextOrError"
 *   4. Keeps running forever (run it in the background with &)
 * STUDENT NAME: Jesus Emilio Salgado Suarez
 * NOTE: This matches the Shakespeare sslookupserver.cpp pattern but for Bible references.
 */

#include <iostream>
#include <string>
#include "Bible.h"
#include "Ref.h"
#include "Verse.h"
#include "fifo.h"

using namespace std;

int main() {
    // Create the Bible object ONCE. This builds the entire inverted index
    Bible bible;   // uses default constructor with 

    cout << "=== Lookup Server STARTED ===" << endl;
    cout << "Index built with " << /* index size is printed by buildIndex() */ " verses" << endl;
    cout << "Waiting for requests on request pipe..." << endl;

    // Pipe names (Fifo class adds the class-specific prefix /home/class/csc3004/tmp/emisalgadosuarez_)
    string reqName = "request";
    string repName = "reply";

    Fifo requestPipe(reqName);
    Fifo replyPipe(repName);

    // Server keeps the request pipe open for reading for the entire lifetime
    requestPipe.openread();

    while (true) {
        // Block until a client (the CGI Lookup Client) sends a request
        string msg = requestPipe.recv();
        cout << "RECEIVED request: " << msg << endl;

        // Parse the request string "book:chapter:verse" into a Ref object
        // Ref constructor handles the format
        Ref ref(msg);

        LookupResult status;
        Verse verse = bible.lookup(ref, status);

        // Prepare reply: always statusNumber|text
        // If successful then verse text; if error then error message from Bible::error()
        string verseText = (status == SUCCESS) ? verse.getVerse() : bible.error(status);
        string replyMsg = to_string(static_cast<int>(status)) + "|" + verseText;

        cout << "SENDING reply: " << replyMsg << endl;

        // Open the reply pipe for writing, send the response, then close it
        replyPipe.openwrite();
        replyPipe.send(replyMsg);
        replyPipe.fifoclose();
    }

    return 0;
}