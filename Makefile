.PHONY: clean

CXXFLAGS= -fpic -Wall -g
LDFLAGS= -shared

BINDIR= build

SOURCES= $(wildcard *.cpp)
OBJECTS= $(patsubst %.cpp,$(BINDIR)/%.o,$(SOURCES))

TARGET_LIB= libsocket.so

all: dir $(BINDIR)/$(TARGET_LIB)

dir:
	mkdir -p $(BINDIR)

clean:
	rm -f $(BINDIR)/*o $(BINDIR)/$(TARGET_LIB)

$(BINDIR)/$(TARGET_LIB): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BINDIR)/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $^ -o $@
