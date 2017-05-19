CXX=g++
CXXFLAGS=-O3
BUILD_DIR=build

hello:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o build/hello hello.cpp

world:
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o build/world world.cpp

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean
