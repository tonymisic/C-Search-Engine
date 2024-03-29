EXE = invert
OBJS = porter2_stemmer.o
CC = g++ -std=c++11 -pedantic -I.
CCOPTS = -O3
LINKER = g++ -std=c++11 -pedantic -I.
OBJ = invert.exe
PARAMS = 1 1
CLASS2 =test.cpp 
OBJ2 =test

all: $(EXE)

$(EXE): $(OBJS) invert.cpp
	$(LINKER) invert.cpp -o $@ $(OBJS)
	./$(OBJ) $(PARAMS)

porter2_stemmer.o: porter2_stemmer.h porter2_stemmer.cpp
	$(CC) $(CCOPTS) -c porter2_stemmer.cpp -o $@
	
test:
	$(CC) -o $(OBJ2) $(CLASS2) $(CFLAGS)
	./$(OBJ2)

search:
	g++ -std=c++11 -pedantic -I. -o search search.cpp $(CFLAGS) porter2_stemmer.o
	./search $(PARAMS)

eval:
	g++ -std=c++11 -pedantic -I. -o eval eval.cpp $(CFLAGS) porter2_stemmer.o
	./eval $(PARAMS)

ui:
	g++ -std=c++11 -pedantic -I. -o ui ui.cpp $(CFLAGS) porter2_stemmer.o
	./ui $(PARAMS)


