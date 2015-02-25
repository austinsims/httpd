CXX = g++ -g -fPIC
NETLIBS= -lnsl -lsocket

all: echo-server echo-client

echo-server : echo-server.o
	$(CXX) -o $@ $@.o $(NETLIBS)

echo-client : echo-client.o
	$(CXX) -o $@ $@.o $(NETLIBS)

use-dlopen: use-dlopen.o
	$(CXX) -o $@ $@.o $(NETLIBS) -ldl

hello.so: hello.o
	ld -G -o hello.so hello.o

%.o: %.cc
	@echo 'Building $@ from $<'
	$(CXX) -o $@ -c -I. $<

clean:
	rm -f *.o use-dlopen hello.so
	rm -f *.o echo-server

