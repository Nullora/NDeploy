CXX = g++
LIBS = -lbcrypt -lz
CXXFLAGS = -std=c++20 -O2 -D_WIN32_WINNT=0x0602 -Icode/include
TARGET = code/build/ndep
SRCS = code/src/main.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

r:
	./$(TARGET) a

a:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)
	./$(TARGET) a