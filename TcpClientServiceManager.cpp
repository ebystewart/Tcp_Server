#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <semaphore.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include "TcpClientServiceManager.h"
#include "TcpServerController.h"
#include "TcpClient.h"
#include <unistd.h>

#define CLIENT_RECV_BUFFER_SIZE 1460 /* This is the max a tcp frame can hold. For Ethernet it is 46 to 1500 Bytes*/
static unsigned char commonRecvBuffer[CLIENT_RECV_BUFFER_SIZE];

static void *tcpClientServiecManagerThreadFn(void *arg);

TcpClientServiceManager::TcpClientServiceManager(TcpServerController *tcpServerController)
{
    this->tcp_ctrlr = tcpServerController;
    this->serviceMgrThread = (pthread_t *)calloc(1, sizeof(pthread_t));
    FD_ZERO(&this->active_fd_set);
    FD_ZERO(&this->backup_fd_set);
    sem_init(&this->semServiceMgr, 0, 0);
    pthread_rwlock_init(&this->rwlock, NULL);
}

void *clientServiecManagerThreadFn(void *arg)
{
    TcpClientServiceManager *tcpClientServiceManager = (TcpClientServiceManager *)arg;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    std::cout << "Client Service Manager Thread Started" << std::endl;
    /* Start thread handler */
    tcpClientServiceManager->startTcpClientServiceManagerThreadHandler();
    return NULL;
}

void TcpClientServiceManager::startTcpClientServiceManagerThread(void)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(this->serviceMgrThread, &attr, clientServiecManagerThreadFn, (void *)this);
    std::cout << "Client Service Manager Thread Created" << std::endl;
    sem_wait(&this->semServiceMgr);
}

void TcpClientServiceManager::startTcpClientServiceManagerThreadHandler(void)
{
    std::cout << "Client Service Manager Thread Handler Entered" << std::endl;
    int opt = 1;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(socklen_t);
    std::list<TcpClient *>::iterator it;
    TcpClient *tcpClient;
    uint32_t recvBytes;
    #if 0
    server_addr.sin_addr.s_addr = htonl(this->tcp_ctrlr->ip_addr);
    server_addr.sin_port = htons(this->tcp_ctrlr->port_no + 1);

    int retVal = connect(this->client_fd, (sockaddr *)&server_addr, sizeof(server_addr));
    if(retVal == -1){
        printf("connect unsuccessful\n");
        exit(0);
    }
    #endif
    /* If initial client fd are closed, they will be reused in new connections
       So, the max fd need not be always the largest number */
    this->max_fd = findMaxFd();
    this->tcp_ctrlr->copyClientToList(&this->tcpClientDb);
    this->copyClientFdToFdSet(&this->backup_fd_set);

    sem_post(&this->semServiceMgr);

    while(true)
    {
        pthread_testcancel();
        this->max_fd = findMaxFd();
        std::cout << "Max fd is " << this->max_fd << std::endl;
        std::cout << "Waiting to receive data from clients..." << std::endl;
        memcpy(&this->active_fd_set, &this->backup_fd_set, sizeof(fd_set));

        if(this->max_fd > 2U)
        {
            select(this->max_fd + 1, &this->active_fd_set, NULL, NULL, NULL);

            for(it = this->tcpClientDb.begin(); it != this->tcpClientDb.end(); it++)
            {
                tcpClient = *it;
                if(FD_ISSET(tcpClient->client_fd, &this->active_fd_set))
                {
                    recvBytes = recvfrom(tcpClient->client_fd, commonRecvBuffer, CLIENT_RECV_BUFFER_SIZE, 0, 
                                    (struct sockaddr *)&client_addr, &addr_len);
                    //std::cout << commonRecvBuffer << std::endl;
                    if(recvBytes){
                        memcpy(&tcpClient->recv_buffer[tcpClient->recvBufferWritePoint], &commonRecvBuffer, recvBytes);
                        if((tcpClient->recvBufferWritePoint + recvBytes) > sizeof(tcpClient->recv_buffer))
                        {
                            tcpClient->recvBufferWritePoint = (recvBytes + tcpClient->recvBufferWritePoint) % 1023U;
                        }
                        else{
                            tcpClient->recvBufferWritePoint += recvBytes;                            
                        }
                        std::cout << tcpClient->recv_buffer << std::endl;
                    }
                }
                /* If any message is to be sent, queue it and send through server contoller thread */
            }
        }
        else{
            sleep(10);
        }
        this->tcp_ctrlr->copyClientToList(&this->tcpClientDb);
        this->copyClientFdToFdSet(&this->backup_fd_set);
    }
}

TcpClientServiceManager::~TcpClientServiceManager(void)
{
    if(!this->serviceMgrThread)
        return;
    pthread_cancel(*this->serviceMgrThread);
    pthread_join(*this->serviceMgrThread, NULL);
    free(this->serviceMgrThread);
    this->serviceMgrThread = NULL;
    this->closeAllConnections();
}

void TcpClientServiceManager::copyClientFdToFdSet(fd_set *fd_set)
{
    TcpClient *tcpClient;
    std::list<TcpClient *>::iterator it;
    for(it = this->tcpClientDb.begin(); it != this->tcpClientDb.end(); it++)
    {
        tcpClient = *it;
        FD_SET(tcpClient->client_fd, fd_set);
    }
}

int TcpClientServiceManager::findMaxFd(void)
{
    TcpClient *tcpClient;
    int temp_fd = 0U;
    std::list<TcpClient *>::iterator it;
    for(it = this->tcpClientDb.begin(); it != this->tcpClientDb.end(); it++)
    {
        tcpClient = *it;
        if(tcpClient->client_fd > temp_fd)
        {
            temp_fd = tcpClient->client_fd;
        }
    }
    return temp_fd;
}

void TcpClientServiceManager::closeAllConnections(void)
{
    this->tcp_ctrlr->closeAllConnections();
}