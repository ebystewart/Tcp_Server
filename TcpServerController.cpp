#include <stdio.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include "TcpServerController.h"
#include "TcpConnectionAcceptor.h"
#include "TcpClientServiceManager.h"
#include "TcpClientDbManager.h"
#include "network_utils.h"
#include "TcpClient.h"


TcpServerController::TcpServerController(std::string ip_addr, uint16_t port_no, std::string server_name)
{
    this->ip_addr     = network_convert_ip_p_to_n(ip_addr.c_str());
    this->port_no     = port_no;
    this->server_name = server_name;

    this->tcpConnectionAcceptor   = new TcpConnectionAcceptor(this);
    this->tcpClientServiceManager = new TcpClientServiceManager(this);
    this->tcpClientDbManager      = new TcpClientDbManager(this);

    pthread_rwlock_init(&this->rwlock, NULL);
    pthread_mutex_init(&this->mesgQMutex, NULL);
    pthread_cond_init(&this->mesgQCondition, NULL);

    this->serverControllerMsgQThread = (pthread_t *)calloc(0, sizeof(pthread_t));
}


TcpServerController::~TcpServerController(void)
{
    std::cout << "TcpServerController's destructor called" << std::endl;
    delete this->tcpConnectionAcceptor;
    delete this->tcpClientServiceManager;
    delete this->tcpClientDbManager;
}

static void *serverControllerMsgQThreadFn(void *arg)
{
    TcpServerController *tcpServerController = (TcpServerController *)arg;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    tcpServerController->MsgQProcessingThreadInternal();
    return NULL;
}

void TcpServerController::StartTcpServer(void)
{
    int retVal = pthread_create(this->serverControllerMsgQThread, NULL, serverControllerMsgQThreadFn, (void *)this);
    if(retVal < 0)
    {
        std::cout << "MsgQ Thread creation Failed" << std::endl;
        exit(0);
    }

}

void TcpServerController::StopTcpServer(void)
{
    
}

void TcpServerController::MsgQProcessingThreadInternal(void)
{
    TcpServerMsg_t *msg;

    std::cout << "The MsgQprocessing thread in the server controller started ..." << std::endl;

    while(true)
    {
        pthread_mutex_lock(&this->mesgQMutex);
        while(this->msgQ.empty())
        {
            pthread_cond_wait(&this->mesgQCondition, &this->mesgQMutex);
        }

        while(1)
        {
            if(this->msgQ.empty())
                break;
            msg = this->msgQ.front();
            this->msgQ.pop_front();
            //this->ProcessMsgQMsg(msg);
            if(msg->zero_sem)
                sem_post(msg->zero_sem);
        }
        pthread_mutex_unlock(&this->mesgQMutex);
    }
}

void TcpServerController::StartTcpConnectionAcceptorSrv(void)
{
    this->tcpConnectionAcceptor->startTcpConnAcceptorThread();
}

void TcpServerController::StartTcpClientServiceManagerSrv(void)
{
    this->tcpClientServiceManager->startTcpClientServiceManagerThread();
}

void TcpServerController::StartTcpClientDbManagerSrv(void)
{
    //this->tcpClientDbManager->startTcpClientDbManagerThread();
}

void TcpServerController::addClientToDB(TcpClient *tcpClient)
{
    this->tcpClientDbManager->AddClientToDb(tcpClient);
}

void TcpServerController::copyClientToList(std::list<TcpClient *> *listDb)
{
    this->tcpClientDbManager->copyClientToList(listDb);
}

void TcpServerController::displayList(void)
{
    this->tcpClientDbManager->displayList();
}

void TcpServerController::closeAllConnections(void)
{
    this->tcpClientDbManager->closeAllConnections();
}