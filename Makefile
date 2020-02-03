GPP = g++
SOURCES = rasterizer.cpp
OPTIONS = -std=c++11 -o
TARGET = rasterizer
LIBS =

all:
	$(GPP) $(SOURCES) $(OPTIONS) $(TARGET) $(LIBS)

clean:
	rm $(TARGET) 
