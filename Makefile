# Compiler and standard
CXX = g++
CXXFLAGS = -std=c++17

# Libraries and flags from pkg-config
PKG_CONFIG = pkg-config
ALLEGRO_LIBS = $(shell $(PKG_CONFIG) allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 allegro_ttf-5 --libs --cflags)

# Source files and output binary
SRC = ECGraphicViewImp.cpp Elevator.cpp Simulate.cpp ECElevatorSim.cpp Passenger.cpp
TARGET = simulate

# Default target
all: $(TARGET)

# Build the target
$(TARGET): $(SRC)
	$(CXX) $(SRC) $(ALLEGRO_LIBS) $(CXXFLAGS) -o $(TARGET)

# Clean up build artifacts
clean:
	rm -f $(TARGET)