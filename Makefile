CXX = g++
CXXFLAGS = -std=c++17 -O2 -Icode/include
TARGET = code/build/ndep
SRCS = code/src/main.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

r:
	./$(TARGET) a

a:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)
	./$(TARGET)