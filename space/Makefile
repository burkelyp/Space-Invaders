CXX = g++
CXXFLAGS = -Wall -std=c++11 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

SRC = main.cpp emulator.cpp sdl_display.cpp input.cpp shift_register.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = invaders

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
