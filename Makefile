all:
	g++ -o sendfile Client.cpp
	g++ -o recvserverd Server.cpp