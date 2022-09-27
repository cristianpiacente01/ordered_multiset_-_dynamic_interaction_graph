CXX = g++ # compilatore utilizzato

# -Wall stampa i messaggi di warning
# -std=c++0x potrebbe servire ad alcuni compilatori non recenti per utilizzare C++11 e quindi supportare nullptr
CXXFLAGS = -Wall -std=c++0x

#MODE = -DNDEBUG # release
MODE = # modalita' debug

main.exe: main.o element_not_found.o 
	$(CXX) $(MODE) $(CXXFLAGS) main.o element_not_found.o -o main.exe

main.o: main.cpp ordered_multiset.h element_not_found.h
	$(CXX) $(MODE) $(CXXFLAGS) -c main.cpp -o main.o

element_not_found.o: element_not_found.cpp element_not_found.h
	$(CXX) $(MODE) $(CXXFLAGS) -c element_not_found.cpp -o element_not_found.o

.PHONY: clean
clean:
	rm -rf *.o *.exe