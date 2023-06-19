INCLUDE = -I../CppServerTools/socket/ -I../CppServerTools/struct/ -I../CppServerTools/epoll/ -I../CppServerTools/packet/basepacket -I../CppServerTools/log/ -Iinfo/ -Istruct/ -Imanager/
SUBDIRS = manager
MODULE = manager/SocketManager.o
RESULT = server
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


#Application
server.o: server.cpp
	$(CC) $(INCLUDE) -c $^