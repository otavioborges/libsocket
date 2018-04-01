.PHONY: clean
.PHONY: install
.PHONY: uninstall

CXXFLAGS= -fPIC -Wall -std=c++11 -g3
LDFLAGS= -shared
BINDIR= build

all: CXXFLAGS+= -DSERVER
client: CXXFLAGS+= -DCLIENT
server: CXXFLAGS+= -DSERVER

SOURCES= $(wildcard *.cpp)
OBJECTS= $(patsubst %.cpp,$(BINDIR)/%.o,$(SOURCES))

CLIENT_OBJS= $(BINDIR)/SocketClient.o
SERVER_OBJS= $(BINDIR)/ClientStruct.o $(BINDIR)/ClientStructSet.o $(BINDIR)/SocketServer.o

CLIENT_HEADERS= SocketClient.h SocketClientCallback.h SocketFamily.h SocketType.h socrates.h
SERVER_HEADERS= ClientStruct.h ClientStructSet.h SocketServer.h SocketServerCallback.h SocketFamily.h SocketType.h socrates.h
LIB_HEADERS= SocketClient.h SocketClientCallback.h  ClientStruct.h ClientStructSet.h SocketServer.h SocketServerCallback.h SocketFamily.h SocketType.h socrates.h

TARGET_LIB= libsocrates.so
TARGET_CLIENT= libsocrates-client.so
TARGET_SERVER= libsocrates-server.so

all: dir $(BINDIR)/$(TARGET_LIB)

client: dir $(BINDIR)/$(TARGET_CLIENT)

server: dir $(BINDIR)/$(TARGET_SERVER)

dir:
	mkdir -p $(BINDIR)

clean:
	rm -f $(BINDIR)/*o $(BINDIR)/*.so

install:
ifneq ("$(wildcard $(BINDIR)/$(TARGET_LIB))", "")
	install -d /usr/lib
	install -m 644 $(BINDIR)/$(TARGET_LIB) /usr/lib
	install -d /usr/include/socrates
	install -m 644 $(LIB_HEADERS) /usr/include/socrates
endif
ifneq ("$(wildcard $(BINDIR)/$(TARGET_CLIENT))", "")
	install -d /usr/lib
	install -m 644 $(BINDIR)/$(TARGET_CLIENT) /usr/lib
	install -d /usr/include/socrates
	install -m 644 $(CLIENT_HEADERS) /usr/include/socrates 
endif
ifneq ("$(wildcard $(BINDIR)/$(TARGET_SERVER))", "")
	install -d /usr/lib
	install -m 644 $(BINDIR)/$(TARGET_SERVER) /usr/lib
	install -d /usr/include/socrates
	install -m 644 $(SERVER_HEADERS) /usr/include/socrates
endif

uninstall:
ifneq ("$(wildcard /usr/lib/$(TARGET_LIB))", "")
	rm /usr/lib/$(TARGET_LIB)
endif
ifneq ("$(wildcard /usr/lib/$(TARGET_CLIENT))", "")
	rm /usr/lib/$(TARGET_CLIENT)
endif
ifneq ("$(wildcard /usr/lib/$(TARGET_SERVER))", "")
	rm /usr/lib/$(TARGET_SERVER)
endif
	rm -rf /usr/include/socrates

$(BINDIR)/$(TARGET_CLIENT): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(BINDIR)/$(TARGET_SERVER): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(BINDIR)/$(TARGET_LIB): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(BINDIR)/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $^ -o $@ -lpthread
