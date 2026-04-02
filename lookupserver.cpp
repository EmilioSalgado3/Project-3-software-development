#include <iostream>
#include <string>
#include "Bible.h"
#include "Ref.h"
#include "Verse.h"
#include "fifo.h"

using namespace std;

int main() {
    // Build the Bible index once
    Bible bible;

    // Create the two pipes
    Fifo requestPipe("request");
    Fifo replyPipe("reply");

    // Keep handling requests
    while (true) {
        // Read one request
        requestPipe.openread();
        string msg = requestPipe.recv();
        requestPipe.fifoclose();

        // Convert request text into a Ref
        Ref ref(msg);

        // Look up verse
        LookupResult status;
        Verse verse = bible.lookup(ref, status);

        // Build reply: status|text
        string replyText;
        if (status == SUCCESS) {
            replyText = verse.getVerse();
        } else {
            replyText = bible.error(status);
        }

        string replyMsg = to_string((int)status) + "|" + replyText;

        // Send reply
        replyPipe.openwrite();
        replyPipe.send(replyMsg);
        replyPipe.fifoclose();
    }

    return 0;
}