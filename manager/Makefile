INCLUDE = -I../../CppServerTools/epoll/ -I../../CppServerTools/socket/ -I../../CppServerTools/struct/ -I../../CppServerTools/epoll/ -I../../CppServerTools/packet/basepacket -I../../CppServerTools/log/ -I../info/ -I../struct/
MAKE_MANAGER = SocketManager.o
CC = g++ -g

#Init
init: create_manager


#Operation
create_manager:
	make $(MAKE_MANAGER)

clean:
	rm *.o


#MAKE_MANAGER
SocketManager.o: SocketManager.cpp
	$(CC) $(INCLUDE) -c $^