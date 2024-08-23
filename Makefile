build: ./src/wavefile.cpp ./src/main.cpp
	g++ -std=c++11 ./src/wavefile.cpp ./src/main.cpp -o ./build/main

run:
	./build/main

clean:
	rm -f ./build/main


dingo:
	g++ -std=c++11 -I/opt/homebrew/include/libxml2 -L/opt/homebrew/lib -lxml2 ./src/mxml_parser.cpp -o ./build/mxml_parser

rundingo:
	./build/mxml_parser
