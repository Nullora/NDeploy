CXX = g++
LIBS = -lbcrypt -lz
CXXFLAGS = -std=c++17 -O2 -Icode/include
TARGET = code/build/ndep
SRCS = code/src/main.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

r:
	./$(TARGET) a

a:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)
	./$(TARGET) a