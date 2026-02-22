starklag: starklag.o
	g++ -o starklag starklag.o -lSista -lpthread

starklag.o: starklag.cpp
	g++ -c starklag.cpp -lSista -lpthread

clean:
	rm -f starklag starklag.o
