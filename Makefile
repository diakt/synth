build: ./src/wavefile.cpp ./src/main.cpp
	g++ -std=c++11 ./src/wavefile.cpp ./src/main.cpp -o ./build/main

run:
	./build/main

clean:
	rm -f ./build/main