#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <iostream>

char buffer[1024]; 
void startClient(void)
{
    char *ip = "127.0.0.1";
    uint16_t port = 4000;
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock_fd < 0){
        printf("client socket creation failed\n");
    }
    else
        std::cout << "Client Socket Created" <<std::endl;
    
    int retVal = connect(sock_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if(retVal < 0){
        std::cout << "Client Connect failed" << std::endl;
        printf("errorcode %d\n",errno);
    }
    else
        std::cout << "Client connected" << std::endl;

    bzero(buffer, 1024); // memset is alternative
    strcpy(buffer, "hello!, this is the client");
    printf("Client: %s\n", buffer);
    retVal = sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&addr, sizeof(addr));
    if(retVal < 0)
        std::cout << "Client failed to sent" << std::endl;
    else
        std::cout << "client sent data successfully" << std::endl;

    //while(true)
    {
        bzero(buffer, 1024);
        /* recvfrom needs server address; if fd is available, address will be ignored, provided NULL is passed */
        retVal = recvfrom(sock_fd, buffer, sizeof(buffer), 0, NULL, 0);//(struct sockaddr *)&addr, 0);
        if(retVal < 0)
            std::cout << "Client failed to receive" << std::endl;
        else{
            printf("Server: %s\n",buffer);
            std::cout << "client received data successfully" << std::endl;
        }  
    }
}

int
main(int argc, char **argv){

    //setup_tcp_communication();
    startClient();
    printf("application quits\n");
    return 0;
}