CXX = g++
CXXFLAGS = -Wall -std=c++11
CGICC_INC = -I/home/class/csc3004/cgicc
CGICC_LIB = -L/home/class/csc3004/cgicc -lcgicc
USER_DIR = /var/www/html/class/csc3004/emisalgadosuarez
CGI_DIR = $(USER_DIR)/cgi-bin

OBJS = Bible.o Ref.o Verse.o fifo.o

all: testreader lookupserver bibleindex.cgi

# Build the Part 1 test program.
testreader: testreader.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o testreader testreader.o $(OBJS)

# Build the lookup server.
lookupserver: lookupserver.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o lookupserver lookupserver.o $(OBJS)

# Build the CGI lookup client.
bibleindex.cgi: bibleindex.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o bibleindex.cgi bibleindex.o $(OBJS) $(CGICC_LIB)

# Compile the Part 1 test program.
testreader.o: testreader.cpp Bible.h Ref.h Verse.h
	$(CXX) $(CXXFLAGS) -c testreader.cpp

# Compile the lookup server.
lookupserver.o: lookupserver.cpp Bible.h Ref.h Verse.h fifo.h
	$(CXX) $(CXXFLAGS) -c lookupserver.cpp

# Compile the CGI lookup client.
bibleindex.o: bibleindex.cpp Bible.h Ref.h Verse.h fifo.h
	$(CXX) $(CXXFLAGS) $(CGICC_INC) -c bibleindex.cpp

# Compile the Bible class.
Bible.o: Bible.cpp Bible.h Ref.h Verse.h
	$(CXX) $(CXXFLAGS) -c Bible.cpp

# Compile the Ref class.
Ref.o: Ref.cpp Ref.h
	$(CXX) $(CXXFLAGS) -c Ref.cpp

# Compile the Verse class.
Verse.o: Verse.cpp Verse.h Ref.h
	$(CXX) $(CXXFLAGS) -c Verse.cpp

# Compile the Fifo class.
fifo.o: fifo.cpp fifo.h
	$(CXX) $(CXXFLAGS) -c fifo.cpp

# Copy the files to the web server folders.
install: bibleindex.cgi lookupserver bibleindex.html
	cp bibleindex.cgi $(CGI_DIR)/
	cp bibleindex.html $(USER_DIR)/

# Remove build files.
clean:
	rm -f *.o testreader lookupserver bibleindex.cgi
