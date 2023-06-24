INCLUDE = -I../../CppServerTools/epoll/ -I../../CppServerTools/socket/ -I../../CppServerTools/struct/ -I../../CppServerTools/epoll/ -I../../CppServerTools/packet/basepacket -I../../CppServerTools/log/ -I../info/ -I../struct/ -I../manager/
MAKE_HANDLER = EchoPacketHandler.o GameRoomHandler.o
CC = g++ -g

#Init
init: create_handler


#Operation
create_handler:
	make $(MAKE_HANDLER)

clean:
	rm *.o


#MAKE_HANDLER
EchoPacketHandler.o: EchoPacketHandler.cpp
	$(CC) $(INCLUDE) -c $^

GameRoomHandler.o: GameRoomHandler.cpp
	$(CC) $(INCLUDE) -c $^