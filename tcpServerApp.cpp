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
void startServer(void)
{
    char *out;
    char *ip = "127.0.0.1";
    uint16_t port = 4000;
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    int attr;
    int accept_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(accept_fd < 0){
        printf("client socket creation failed\n");
    }
    else
        std::cout << "Client Socket Created" <<std::endl;
    
        int opt = 1;

        if(setsockopt(accept_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0U)
        {
             printf("%s(): Setting of socket option to reuse address failed.\n", __FUNCTION__);
             exit(0);
        }
        if(setsockopt(accept_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt)) > 0U)
        {
            printf("%s(): Setting of sock option to reuse port address failed\n", __FUNCTION__);
            exit(0);
        }
    
        if(bind(accept_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
        {
            printf("%s(): socket binding failed [%s(0x%x), %d] with errorCode %d\n",__FUNCTION__, 
                     ip,
                    inet_pton(AF_INET, ip, NULL), port, errno);
            exit(0);
        }
    
        if(listen(accept_fd, 5) < 0U)
        {
            printf("listen failed \n");
            exit(0);
        }
        else{
            printf("tcp server is up and running: [%s:%d]\n", ip, port);
        }
        /* define a client who would try to connect */
        struct sockaddr_in client_addr;
        socklen_t clientAddrLen = sizeof(client_addr);
        int sock_fd;
        socklen_t recvLength;
    
        while(true)
        {
            pthread_testcancel();
            sock_fd = accept(accept_fd, (struct sockaddr *)&client_addr, &clientAddrLen);
    
            if(sock_fd < 0U)
            {
                std::cout << "Error in accepting new connection" << std::endl;
                continue;
            }
            else{
                printf("Conn accepted from client: [%s:%d]\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, out, 16), client_addr.sin_port);
            }
    
            ssize_t numBytes = sendto(sock_fd, "Welcome!", sizeof("Welcome!"), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
            if(numBytes > 0)
                 std::cout << "Server sending welcome message to client..." << std::endl;
            else
                 std::cout << "Server message sending failed..."  << std::endl;

            bzero(buffer, 1024);
            numBytes = recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, (socklen_t *)&recvLength);
            if(numBytes < 0)
                std::cout << "Reception from client failed" << std::endl;
            else{
                printf("Server: %s\n",buffer);
                std::cout << "Message from client received successfully" << std::endl;
            }
        }   
}

int
main(int argc, char **argv){

    startServer();
    printf("application quits\n");
    return 0;
}