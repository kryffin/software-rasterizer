GPP = g++
SOURCES = rasterizer.cpp
OPTIONS = -o
TARGET = rasterizer
LIBS =

all:
	$(GPP) $(SOURCES) $(OPTIONS) $(TARGET) $(LIBS)

clean:
	rm $(SOURCES) 