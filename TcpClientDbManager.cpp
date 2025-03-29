
#include <assert.h>
#include "TcpClientDbManager.h"
#include "TcpClient.h"
#include "network_utils.h"
#include "unistd.h"


TcpClientDbManager::TcpClientDbManager(TcpServerController *tcpServerController)
{
    this->tcpServerController = tcpServerController;
}

TcpClientDbManager::~TcpClientDbManager(void)
{

}

TcpClient * TcpClientDbManager::LookUpClientDb(uint32_t ipAddr, uint16_t portNum)
{
    TcpClient *tcpClient;

    std::list<TcpClient *>::iterator it;

    for(it = this->tcpClientDb.begin(); it != this->tcpClientDb.end(); it++)
    {
        tcpClient = *it;
        if(tcpClient->client_ip_addr == ipAddr && tcpClient->client_port_no == portNum)
        {
            return tcpClient;
        }
    }
    return NULL;
}

void TcpClientDbManager::AddClientToDb(TcpClient *tcpClient)
{
    pthread_rwlock_rdlock(&this->rwlock);
    assert(!(this->LookUpClientDb(tcpClient->client_ip_addr, tcpClient->client_port_no)));
    this->tcpClientDb.push_back(tcpClient);
    pthread_rwlock_unlock(&this->rwlock);
}

void TcpClientDbManager::RemoveClientFromDb(TcpClient *tcpClient)
{
    pthread_rwlock_rdlock(&this->rwlock);
    this->tcpClientDb.remove(tcpClient);
    pthread_rwlock_unlock(&this->rwlock);
}

void TcpClientDbManager::UpdateClient(TcpClient *)
{

}

void TcpClientDbManager::copyClientToList(std::list<TcpClient *> *listDb)
{
    TcpClient *tcpClient;
    std::list<TcpClient *>::iterator it;

    for(it = this->tcpClientDb.begin(); it != this->tcpClientDb.end(); it++)
    {
        tcpClient = *it;
        listDb->push_back(tcpClient);       
        this->incrementClientRefCount(tcpClient);
    }
}

void TcpClientDbManager::clearList(void)
{
    TcpClient *tcpClient;
    std::list<TcpClient *>::iterator it;

    for(it = this->tcpClientDb.begin(); it != this->tcpClientDb.end(); it++)
    {
        tcpClient = *it;    
        this->decrementClientRefCount(tcpClient);
        this->tcpClientDb.pop_back();   
    }
}

void TcpClientDbManager::displayList(void)
{
    TcpClient *tcpClient;
    std::list<TcpClient *>::iterator it;
    int i = 0U;
    pthread_rwlock_rdlock(&this->rwlock);
    for(it = this->tcpClientDb.begin(); it != this->tcpClientDb.end(); it++)
    { 
        i++;
        tcpClient = *it;
        printf("Connected Client #%d: [%s, %d]\n", i, network_convert_ip_n_to_p(tcpClient->client_ip_addr, nullptr), 
                                                    tcpClient->client_port_no);
    }
    pthread_rwlock_unlock(&this->rwlock);
}

void TcpClientDbManager::closeAllConnections(void)
{
    TcpClient *tcpClient;
    std::list<TcpClient *>::iterator it;

    for(it = this->tcpClientDb.begin(); it != this->tcpClientDb.end(); it++)
    {
        tcpClient = *it;
        if(tcpClient->client_fd > 3U)
            close(tcpClient->client_fd);
        free(tcpClient);
    }
}

void TcpClientDbManager::incrementClientRefCount(TcpClient *tcpClient)
{
    tcpClient->ref_count++;
}

void TcpClientDbManager::decrementClientRefCount(TcpClient *tcpClient)
{
    tcpClient->ref_count--;
}