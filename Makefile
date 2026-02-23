starklag: starklag.o
	g++ -o starklag starklag.o -lSista -lpthread

starklag.o: starklag.cpp stats.cpp organism.cpp dna.cpp
	g++ -c starklag.cpp -lSista -lpthread

editor: editor.o
	g++ -o editor editor.o -lSista -lpthread

editor.o: editor.cpp organism.cpp
	g++ -c editor.cpp -lSista -lpthread

clean:
	rm -f starklag starklag.o
