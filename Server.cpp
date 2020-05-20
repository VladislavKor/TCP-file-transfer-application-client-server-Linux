#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8888
#define BUFFER_SIZE 1024

using namespace std;

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;

    int readed;
    char buff[BUFFER_SIZE];
    ofstream out;
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        cerr << "socket failed\n"; 
        exit(EXIT_FAILURE); 
    } 

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    { 
        cerr << "bind failed\n"; 
        exit(EXIT_FAILURE); 
    }
    
    if (listen(server_fd, 5) < 0) 
    { 
        cerr << "listen\n"; 
        exit(EXIT_FAILURE); 
    } 

    cout << "Server is running...\n";
    while (1)
    {
        new_socket = accept(server_fd, NULL, NULL);
        if (new_socket < 0)
        {
            cerr << "accept\n"; 
            exit(EXIT_FAILURE); 
        }
        
        while (1)
        {
            readed = read(new_socket, buff, BUFFER_SIZE);
            if  (readed > 0)
            {
                if (!out.is_open()) // создание / открытие файла
                {
                    out.open(buff);
                    if (!out.is_open())
                    {
                        cerr << "file opening\n"; 
                        exit(EXIT_FAILURE); 
                    }
                    cout << "Open file: " << buff << endl;
                }
                else    // запись данных
                {
                    cout << buff << endl;
                    out.write(buff, readed);
                }      
            }
            else if (readed == 0)   // закрытие файла
            {
                if (out.is_open())
                {
                    out.close();
                    cout << "file closed\n";
                    break;
                }
            }
            else
            {
                cerr << "read\n";
                exit(EXIT_FAILURE);
            }   
        }

        close(new_socket);
    }
}