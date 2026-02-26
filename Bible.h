#ifndef Bible_H
#define Bible_H

#include "Ref.h"
#include "Verse.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>

using namespace std;

enum LookupResult { SUCCESS, NO_BOOK, NO_CHAPTER, NO_VERSE, OTHER };

class Bible
{   
   private:
      string infile;
      ifstream instream;
      bool isOpen;

      map<Ref,int> index;
      map<Ref,int>::iterator currentPos;

      void buildIndex();

   public:
      Bible();
      Bible(const string s);
   
      Verse lookup(const Ref ref, LookupResult& status);
      Verse nextVerse(LookupResult& status);

      string error(LookupResult status);
      void display();
   
      Ref next(const Ref ref, LookupResult& status);
      Ref prev(const Ref ref, LookupResult& status);
};

#endif
