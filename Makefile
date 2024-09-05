CXX = g++
CXXFLAGS = -std=c++17 -I/opt/homebrew/include/libxml2
LDFLAGS = -L/opt/homebrew/lib -lxml2

SRC_DIR = ./src
BUILD_DIR = ./build

SRCS = $(SRC_DIR)/mxml_parser.cpp $(SRC_DIR)/main.cpp $(SRC_DIR)/rfxwavefile.cpp #$(SRC_DIR)/wavefile.cpp
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

