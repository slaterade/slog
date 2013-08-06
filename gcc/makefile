# amateur attempt at a makefile

SHELL = /bin/sh
CXX = g++
FLAGS = -std=c++11 -stdlib=libc++
CXXFLAGS = -Wall

TARGET = slog
SOURCE = $(shell echo ../source/*.cpp)
INCLUDE_PATH = ../include

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(FLAGS) $(CXXFLAGS) -o $(TARGET) $(SOURCE) -I$(INCLUDE_PATH)

clean:
	-rm -f $(TARGET)
