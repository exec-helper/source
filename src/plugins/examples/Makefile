CXX=g++
CXXFLAGS=-O3
SRC_DIR=src
BUILD_DIR=build

hello:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/hello $(SRC_DIR)/hello.cpp

world:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/world $(SRC_DIR)/world.cpp

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean
