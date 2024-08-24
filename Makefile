CXX = g++
CXXFLAGS = -std=c++11 -I/opt/homebrew/include/libxml2
LDFLAGS = -L/opt/homebrew/lib -lxml2

SRC_DIR = ./src
BUILD_DIR = ./build

SRCS = $(SRC_DIR)/wavefile.cpp $(SRC_DIR)/mxml_parser.cpp $(SRC_DIR)/main.cpp
OBJS = $(patsubst $(SRC_DIR)%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BUILD_DIR)/main


build: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)



# build: ./src/wavefile.cpp ./src/main.cpp
# 	g++ -std=c++11 ./src/wavefile.cpp ./src/main.cpp -o ./build/main

# run:
# 	./build/main

# clean:
# 	rm -f ./build/main


# dingo:
# 	g++ -std=c++11 -I/opt/homebrew/include/libxml2 -L/opt/homebrew/lib -lxml2 ./src/mxml_parser.cpp -o ./build/mxml_parser

# rundingo:
# 	./build/mxml_parser
