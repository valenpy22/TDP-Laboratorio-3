FLAGS = -g

all: main

main: Node.o Heap.o BB.o Simplex.o main.cpp
	g++ $(FLAGS) Node.o Heap.o BB.o Simplex.o main.cpp -o main

Simplex.o: Simplex.cpp Simplex.h
	g++ $(FLAGS) -c Simplex.cpp

Node.o: Node.cpp Node.h
	g++ $(FLAGS) -c Node.cpp

Heap.o: Heap.cpp Heap.h
	g++ $(FLAGS) -c Heap.cpp

BB.o: BB.cpp BB.h
	g++ $(FLAGS) -c BB.cpp

clean:
	rm -f *.o main
