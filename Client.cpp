#include <iostream>
#include <cstring>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PORT 8888

using namespace std;

int main(int argc, char* argv[])
{
    char buff[BUFFER_SIZE];
    int sock;
    struct sockaddr_in address;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        cerr << "socket failed\n"; 
        return(1); 
    } 


    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            cout << "Использование: sendfile [КЛЮЧ] ФАЙЛ\n";
            cout << "Осуществляет предачу файла на сервер\n\n";
            cout << "\t--help\tпоказать эту справку и выйти\n";
        }
        else
        {
            ifstream in;
            in.open(argv[1]);
            if (!in.is_open())
            {
                cerr << "Файл с таким именем не найден\n";
                return 1;
            }

            // создание сокета
            address.sin_family = AF_INET;
            address.sin_port = htons(PORT);
            address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            if (connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
            {
                cerr << "connect\n";
                return 1;
            }
            
            // отправка имени файла
            if (write(sock, argv[1], strlen(argv[1]) + 1) < 0)
            {
                cerr << "write\n";
                return 1;
            }

            // отправка всего файла
            while (in)
            {
                in.read(buff, BUFFER_SIZE);
                if (write(sock, buff, in.gcount()) < 0)
                {
                    cerr << "write\n";
                    return 1;
                }
            }
            
            
            in.close();
            close(sock);

            cout << "Файл " << argv[1] << " отправлен\n";
        }

    }
    else
    {
        cout << "sendfile: неверное число операндов" << endl;
        cout << "По команде «sendfile --help» можно получить дополнительную информацию." << endl;
    }

    return 0;
}