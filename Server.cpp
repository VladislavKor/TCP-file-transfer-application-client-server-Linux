//#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define DIR "/home/koromyslov"

using namespace std;

int server_fd, new_socket;
ofstream out;

void signal_handler(int signal)
{
    if  (out.is_open())
        out.close();
    close(new_socket);
    syslog (LOG_NOTICE, "recvserver daemon terminated");
    closelog();
    exit(EXIT_SUCCESS);
}

static void daemonize()
{
    pid_t pid;

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);

    chdir(DIR);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    openlog ("recvserverd", LOG_PID, LOG_DAEMON);
    //closelog(); /////////////////////////
}

int main()
{

    daemonize();

    syslog (LOG_NOTICE, "recvserver daemon started");

    //-------------------------------
    //int server_fd, new_socket;
    struct sockaddr_in address;

    int readed;
    char buff[BUFFER_SIZE];
    //ofstream out;
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        syslog (LOG_ERR, "socket failed");
        exit(EXIT_FAILURE); 
    } 

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    { 
        syslog (LOG_ERR, "bind failed"); 
        exit(EXIT_FAILURE); 
    }
    
    if (listen(server_fd, 5) < 0) 
    { 
        syslog (LOG_ERR, "listen"); 
        exit(EXIT_FAILURE); 
    } 

    while (1)
    {
        new_socket = accept(server_fd, NULL, NULL);
        if (new_socket < 0)
        {
            syslog (LOG_ERR, "accept"); 
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
                        syslog (LOG_ERR, "file opening"); 
                        exit(EXIT_FAILURE); 
                    }
                }
                else    // запись данных
                {
                    out.write(buff, readed);
                }      
            }
            else if (readed == 0)   // закрытие файла
            {
                if (out.is_open())
                {
                    out.close();
                    break;
                }
            }
            else
            {
                syslog (LOG_ERR, "read");
                exit(EXIT_FAILURE);
            }   
        }

        close(new_socket);
    }
}