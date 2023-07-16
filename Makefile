INCLUDE = -I../CppServerTools/socket/ -I../CppServerTools/struct/ -I../CppServerTools/epoll/ -I../CppServerTools/packet/basepacket -I../CppServerTools/log/ -Iinfo/ -Istruct/ -Imanager/ -Ithread/ -Ihandler/
SUBDIRS = manager thread handler
MODULE = manager/SocketManager.o thread/ServerThread.o handler/UserPacketHandler.o handler/GameRoomHandler.o
RESULT = server test_client
CC = g++ -g


#Init
init: create_main


#Operation
create_main:
	for DIR in $(SUBDIRS); do \
		$(MAKE) -C $$DIR; \
	done
	make $(RESULT)

clean: 
	for DIR in $(SUBDIRS); do \
		$(MAKE) -C $$DIR clean; \
	done
	rm *.o
	rm $(RESULT)


#Main File
server: server.o $(MODULE)
	$(CC) -o $@ $^ -L../CppServerTools/ -lserver_tools

test_client: test_client.o $(MODULE)
	$(CC) -o $@ $^ -L../CppServerTools/ -lserver_tools

#Application
server.o: server.cpp
	$(CC) $(INCLUDE) -c $^

test_client.o: test_client.cpp
	$(CC) $(INCLUDE) -c $^