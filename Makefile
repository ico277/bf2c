CXX=c++
SOURCES=bf2c.cpp
CXXFLAGS=-O3
EXECUTABLE=bf2c
PREFIX=/usr/local/bin

all: build
	./$(EXECUTABLE) --version

build:
	$(CXX) $(SOURCES) $(CXXFLAGS) -o $(EXECUTABLE)
	
install: build
	cp $(EXECUTABLE) $(PREFIX)/

uninstall:
	rm -f $(PREFIX)/$(EXECUTABLE)

exp: build
	./examples.sh

clean: $(EXECUTABLE)
	rm -f $(EXECUTABLE) examples/*.out

