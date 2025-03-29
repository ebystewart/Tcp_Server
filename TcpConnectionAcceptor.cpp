#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <memory.h>
#include <assert.h>
#include <pthread.h>
#include <thread>
#include <mutex>
#include "TcpConnectionAcceptor.h"
#include "TcpServerController.h"
#include "network_utils.h"
#include "TcpClient.h"

static void *connAcceptorThread(void *arg);


TcpConnectionAcceptor::TcpConnectionAcceptor(TcpServerController *tcpServerController)
{
    this->accept_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(this->accept_fd < 0U)
    {
        std::cout << "Socket Creation Failed" <<std::endl;
        exit(0);
    }
    else
        printf("socket creation successful with fd %d\n", this->accept_fd);

    this->connAcceptorThread = (pthread_t *)calloc(1, sizeof(pthread_t));
    this->connAcceptorFlag = true;
    this->tcp_ctrlr = tcpServerController;

    sem_init(&this->semConnAcceptor, 0, 0);
    pthread_rwlock_init(&this->rwlock, NULL);
}

TcpConnectionAcceptor::~TcpConnectionAcceptor(void)
{
    std::cout << "TcpConnectionAcceptor's destructor called" << std::endl;
    this->stopTcpConnAcceptorThread();
    assert(this->accept_fd == 0U);
    assert(!this->connAcceptorThread);
}

static void *connAcceptorThreadFn(void *arg)
{
    TcpConnectionAcceptor *connAcceptor = (TcpConnectionAcceptor *)arg;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    //std::cout << "TCP Connection Acceptor Thread started" << std::endl;
    /* Call the handler implementing the connection acceptance logic.
       This is to make sure all the private elements are accessible 
       inside the handler.
       The thread function not being part of the class, may not have 
       access to class elements that are private. */
    connAcceptor->tcpConnAcceptorThreadHandler();

    return NULL;
}

void TcpConnectionAcceptor::startTcpConnAcceptorThread(void)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    if(pthread_create(this->connAcceptorThread, &attr, connAcceptorThreadFn, (void *)this))
    {
         printf("%s(): Thread creation failed with errorcode %d\n", __FUNCTION__, errno);
         exit(0);
    }
    std::cout << "TCP Connection Acceptor Thread created" << std::endl;
    /* Set a cancellation point using semaphore */
    sem_wait(&this->semConnAcceptor);
}

void TcpConnectionAcceptor::stopTcpConnAcceptorThread(void)
{
    pthread_cancel(*this->connAcceptorThread);
    pthread_join(*this->connAcceptorThread, NULL);
    free(this->connAcceptorThread);
    this->connAcceptorThread = NULL;

    close(this->accept_fd);
    this->accept_fd = 0;
    sem_destroy(&this->semConnAcceptor);
    pthread_rwlock_destroy(&rwlock);
    //delete this;
}

void TcpConnectionAcceptor::tcpConnAcceptorThreadHandler(void)
{
    std::cout << "The connection acceptor thread started..." << std::endl;
    int opt = 1;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(this->tcp_ctrlr->ip_addr);
    server_addr.sin_port = htons(this->tcp_ctrlr->port_no);

    if(setsockopt(this->accept_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0U)
    {
         printf("%s(): Setting of socket option to reuse address failed.\n", __FUNCTION__);
         exit(0);
    }
    if(setsockopt(this->accept_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt)) > 0U)
    {
        printf("%s(): Setting of sock option to reuse port address failed\n", __FUNCTION__);
        exit(0);
    }

    if(bind(this->accept_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("%s(): socket binding failed [%s(0x%x), %d] with errorCode %d\n",__FUNCTION__, 
                 network_convert_ip_n_to_p(this->tcp_ctrlr->ip_addr, 0U),
                tcp_ctrlr->ip_addr, tcp_ctrlr->port_no, errno);
        exit(0);
    }
    else
        printf("bind successful [%s(0x%x), %d]\n", 
            network_convert_ip_n_to_p(server_addr.sin_addr.s_addr, 0U), ntohs(server_addr.sin_port));

    if(listen(accept_fd, 5) < 0U)
    {
        printf("listen failed \n");
        exit(0);
    }
    else{
        printf("tcp server is up and running: [%s:%d]\n", network_convert_ip_n_to_p(this->tcp_ctrlr->ip_addr, 0U), this->tcp_ctrlr->port_no);
    }
    /* define a client who would try to connect */
    struct sockaddr_in client_addr;
    socklen_t clientAddrLen = sizeof(client_addr);
    int sock_fd;

    sem_post(&this->semConnAcceptor);

    while(true)
    {
        pthread_testcancel();
        /*fd_set *set = (fd_set *)calloc(0, sizeof(fd_set));
        FD_SET(3, set);
        FD_SET(4, set);
        FD_SET(5, set);
        FD_SET(6, set);
        FD_SET(7, set);     
        int val = select(5, set, NULL, NULL, NULL);*/
        std::cout << "Waiting to accept new clients..." << std::endl;
        //printf("accept fd is %d\n",this->accept_fd);
        sock_fd = accept(this->accept_fd, (struct sockaddr *)&client_addr, &clientAddrLen);
        printf("Accept() returned an fd %d...\n",sock_fd);
        if(sock_fd < 0U)
        {
            std::cout << "Error in accepting new connection" << std::endl;
            continue;
        }
        else{
            printf("Conn accepted from client: [%s:%d]\n", network_convert_ip_n_to_p(client_addr.sin_addr.s_addr, 0U), client_addr.sin_port);
        }

        pthread_rwlock_rdlock(&rwlock);
        connAcceptorFlag = this->connAcceptorFlag;
        pthread_rwlock_unlock(&rwlock);

        if(!connAcceptorFlag)
        {
            close(sock_fd);
            continue;
        }

        TcpClient *tcpClient = new TcpClient();
        tcpClient->client_fd = sock_fd;
        tcpClient->client_ip_addr = htonl(client_addr.sin_addr.s_addr);
        tcpClient->client_port_no = htons(client_addr.sin_port);
        tcpClient->tcpServerController = this->tcp_ctrlr;
        tcpClient->server_ip_addr = this->tcp_ctrlr->ip_addr;
        tcpClient->server_port_no = this->tcp_ctrlr->port_no;

        tcpClient->setState(TCP_CLIENT_CONNECTED | TCP_CLIENT_PASSIVE_OPENER);

        pthread_rwlock_wrlock(&rwlock);
        this->tcp_ctrlr->addClientToDB(tcpClient);
        pthread_rwlock_unlock(&rwlock);
        this->tcp_ctrlr->displayList();

        tcpClient->SendMsg("Welcome!\n", sizeof("Welcome!\n"));
        std::cout << "client connected ..." << std::endl;
    }
    //delete tcpClient;
}


void TcpConnectionAcceptor::setConnAcceptorFlag(bool status)
{
    pthread_rwlock_wrlock(&this->rwlock);
    this->connAcceptorFlag = status;
    pthread_rwlock_unlock(&this->rwlock);
}