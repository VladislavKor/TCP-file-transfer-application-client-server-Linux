all:
	g++ -o sendfile Client.cpp
	g++ -o recvserver Server.cpp