//STUDENT NAME: Jesus Emilio Salgado Suarez

#include "Bible.h"
#include "Ref.h"
#include "Verse.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 4 || argc > 5) {
        cout << "Usage: ./testreader book chapter verse [count]" << endl;
        return 1;
    }

    int book = atoi(argv[1]);
    int chap = atoi(argv[2]);
    int vers = atoi(argv[3]);
    int count = (argc == 5 ? atoi(argv[4]) : 1);

    Bible bible("/home/class/csc3004/Bibles/web-complete");

    Ref ref(book, chap, vers);
    LookupResult status;

    Verse v = bible.lookup(ref, status);

    if (status != SUCCESS) {
        cout << "Lookup failed: " << bible.error(status) << endl;
        return 1;
    }

    cout << ref.getBookName() << " " << chap << endl;
    cout << vers << " " << v.getVerse() << endl;

    for (int i = 1; i < count; i++) {
        Verse next = bible.nextVerse(status);
        Ref r = next.getRef();

        if (status != SUCCESS) break;

        if (r.getChapter() != chap) {
            chap = r.getChapter();
            cout << endl << ref.getBookName() << " " << chap << endl;
        }

        cout << r.getVerse() << " " << next.getVerse() << endl;
    }

    return 0;
}
