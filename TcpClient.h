#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <iostream>
#include <stdint.h>
#include <pthread.h>

/* TCP Client States */
#define TCP_CLIENT_CONNECT_IN_PROGRESS  1U
#define TCP_CLIENT_CONNECTED            2U
#define TCP_CLIENT_PASSIVE_OPENER       4U
#define TCP_CLIENT_ACTIVE_OPENER        8U
/*
#define TCP_CLIENT_KA_BASED 32
#define TCP_CLIENT_KA_EXPIRED   64
#define TCP_CLIENT_MULTIPLEX_LISTEN 128
#define TCP_CLIENT_THREADED 256 */

typedef uint32_t clientStateFlag_t;

#define MAX_CLIENT_BUFFER_SIZE 2920U

class TcpServerController;
class TcpClientServiceManager;

class TcpClient
{
    private:
    
        pthread_rwlock_t rwlock;
        clientStateFlag_t stateFlag;

    public:

        uint32_t client_ip_addr;
        uint16_t client_port_no;
        uint32_t server_ip_addr;
        uint16_t server_port_no;

        int client_fd;
        int ref_count;
        uint32_t sentBytes;
        uint32_t receivedBytes;
        TcpServerController *tcpServerController;
        pthread_t *clientDataRecptionThread;
        pthread_t *clientConnectionRetryThread;

        unsigned char recv_buffer[MAX_CLIENT_BUFFER_SIZE];
        uint16_t recvBufferReadPoint;
        uint16_t recvBufferWritePoint;

        TcpClient(void);
        TcpClient(TcpClient *tcpClient);
        TcpClient(uint32_t client_ip_addr, uint16_t client_port_no);

        ~TcpClient(void);

        int SendMsg(char *msg, uint32_t msgSize);

        void setState(clientStateFlag_t stateFlag);

};

#endif