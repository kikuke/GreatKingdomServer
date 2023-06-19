INCLUDE = -I../../CppServerTools/epoll/ -I../../CppServerTools/socket/ -I../../CppServerTools/struct/ -I../../CppServerTools/epoll/ -I../../CppServerTools/packet/basepacket -I../../CppServerTools/log/ -I../info/ -I../struct/ -I../manager/
MAKE_THREAD = ServerThread.o
CC = g++ -g

#Init
init: create_thread


#Operation
create_thread:
	make $(MAKE_THREAD)

clean:
	rm *.o


#MAKE_THREAD
ServerThread.o: ServerThread.cpp
	$(CC) $(INCLUDE) -c $^