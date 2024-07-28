build: wavefile.cpp main.cpp
	g++ -std=c++11 wavefile.cpp main.cpp -o main

run:
	./main

clean:
	rm -f wavegenerator