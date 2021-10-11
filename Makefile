CXX=g++
SOURCES=bf2c.cpp
CXXFLAGS=-O3
EXECUTABLE=bf2c
PREFIX=/usr/local/bin

all: build
	./$(EXECUTABLE)

build:
	$(CXX) $(SOURCES) $(CXXFLAGS) -o $(EXECUTABLE)
	
install: build
	cp $(EXECUTABLE) $(PREFIX)/

uninstall:
	rm -f $(PREFIX)/$(EXECUTABLE)

clean: $(EXECUTABLE)
	rm -f $(EXECUTABLE)

