CC=g++
CFLAGS =--std=c++11
DEPS =
CLASS =invert.cpp 
OBJ =invert
CLASS2 =test.cpp 
OBJ2 =test
PARAMS = 1
all: 
	$(CC) -o $(OBJ) $(CLASS) $(CFLAGS)

run-invert:
	$(CC) -o $(OBJ) $(CLASS) $(CFLAGS)
	./$(OBJ) $(PARAMS)

run-test:
	$(CC) -o $(OBJ2) $(CLASS2) $(CFLAGS)
	./$(OBJ2) $(PARAMS)
	